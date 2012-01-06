/*
 * Trie.cpp
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#include "SystemParams.h"
#include "TrieNode.h"
#include "MMapedFile.h"
#include "BinFile.h"
#include "BinFileMap.h"

#include "Trie.h"


#include <cstring>
#include <cassert>
#include <iostream>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

namespace Db {

template<typename ValueType>
Trie<ValueType>::Trie(BinFile<TrieNode<ValueType> > *nodes, BinFile<TrieLeaf<ValueType> > *leaves)
    : nodes(nodes), leaves(leaves)
{

    switch (leaves->openMMapedFile()) {
    case MMapedFile::OPENED:
        break;
    case MMapedFile::NEW_FILE:
        //initializeEmpty();
        break;
    case MMapedFile::ERROR:
    default:
        cerr << "Opening leaves file failed" << endl;
        break;
    }

    //cout << "Index leaves file opened" << endl;

    switch (nodes->openMMapedFile()) {
    case MMapedFile::OPENED:
        break;
    case MMapedFile::NEW_FILE:
        initializeEmpty();
        break;
    case MMapedFile::ERROR:
    default:
        cerr << "Opening trie file failed" << endl;
        break;
    }

    //cout << "Index file opened" << endl;

}

template<typename ValueType>
Trie<ValueType>::~Trie()
{
    nodes->closeMMapedFile();
    leaves->closeMMapedFile();
}

template<typename ValueType>
void Trie<ValueType>::initializeEmpty()
{
    //cout << "Initializing empty trie file" << endl;

    unsigned long long rootNodeId = nodes->getNewBinByID();
    assert(rootNodeId == 0);
    TrieNode<ValueType> *rootNode = nodes->getBin(rootNodeId);

    unsigned long long leafID = leaves->getNewBinByID();

    if (leafID == -1) {
        cerr << __FILE__ << ":" << __LINE__ << " Couldn't fetch a new leaf" << endl;\
        return;
    }

    rootNode->setChildrenRange(0x00, 0xff, TriePointer(true, leafID));
}

template<typename ValueType>
ValueType Trie<ValueType>::get(const DatabaseKey &key)
{
    int currentCharIdx = 0;

    TrieNode<ValueType> *currentNode = nodes->getBin(0);

    while (true) {
        TriePointer *currentPointer = &currentNode->children[key.data[currentCharIdx]];

        if (currentPointer->leaf == 0) {
            currentCharIdx++;
            currentNode = nodes->getBin(currentPointer->link);

            if (currentCharIdx == key.length) {
                return currentNode->values[key.data[currentCharIdx - 1]];
            }
        } else {
            TrieLeaf<ValueType> *leaf = leaves->getBin(currentPointer->link);
            return leaf->get(key, currentCharIdx);
        }
    }

    cerr << __FILE__ << ":" << __LINE__ << " Reached line that should not be reached" << endl;
    return 0;
}

template<typename ValueType>
void Trie<ValueType>::addKey(const DatabaseKey &key, ValueType value)
{
    int currentCharIdx = 0;

    TrieNode<ValueType> *currentNode = nodes->getBin(0); // root node always has id = 0
    //TriePointer *currentPointer = &currentNode->children[key.data[currentCharIdx]];

    while (true) {
        bool isLinkPure = currentNode->isLinkPure(key.data[currentCharIdx]);
        TriePointer *currentPointer = &currentNode->children[key.data[currentCharIdx]];

        if (currentPointer->isNull()) {
            unsigned char leftmostCharWithCurrentLink = currentNode->checkLeftmostCharWithLink(key.data[currentCharIdx],
                                                                                               *currentPointer);
            unsigned char rightmostCharWithCurrentLink = currentNode->checkRightmostCharWithLink(key.data[currentCharIdx],
                                                                                                 *currentPointer);

            unsigned long long newLeafId = leaves->getNewBinByID();
            TrieLeaf<ValueType> *newLeaf = leaves->getBin(newLeafId);

            currentNode->setChildrenRange(leftmostCharWithCurrentLink, rightmostCharWithCurrentLink,
                                          TriePointer(true, newLeafId));

            newLeaf->add(key, currentCharIdx, value);
            return;
        } else
        if (currentPointer->leaf == 0) {
            currentCharIdx++;
            currentNode = nodes->getBin(currentPointer->link);
            //currentPointer = &currentNode->children[key.data[currentCharIdx]];

            if (currentCharIdx == key.length) {
                /* The whole word had been eaten, and we haven't reached any leaf
                 * node yet. We'll set the value pointer in the non-leaf node.
                 */

                currentNode->values[key.data[currentCharIdx - 1]] = value;
                return;
            }
        } else {
            TrieLeaf<ValueType> *leaf = leaves->getBin(currentPointer->link);

            if (leaf->canFit(key, currentCharIdx)) {
                leaf->add(key, currentCharIdx, value);
                return;
            } else
            if (isLinkPure) {
                unsigned long long newNodeValue = leaf->stripLeadingCharacter();

                TriePointer newPointer;
                if (leaf->isEmpty()) {
                    leaves->freeBin(currentPointer->link);
                } else {
                    newPointer.leaf = 1;
                    newPointer.link = currentPointer->link;
                }

                unsigned long long newNodeIndex = nodes->getNewBinByID();
                TrieNode<ValueType> *newNode = nodes->getBin(newNodeIndex);
                currentPointer->leaf = 0;
                currentPointer->link = newNodeIndex;
                newNode->values[key.data[currentCharIdx]] = newNodeValue;
                newNode->setChildrenRange(0x00, 0xff, newPointer);

                currentCharIdx++;
                currentNode = newNode;

                if (currentCharIdx == key.length) {
                    /* It might happen that the word ends exactly at the fresh node
                     * TODO: we overwrite value
                     */
                    currentNode->values[key.data[currentCharIdx - 1]] = value;
                    return;
                }
            } else {
                unsigned long long newLeafId = leaves->getNewBinByID();
                TrieLeaf<ValueType> *newLeaf = leaves->getBin(newLeafId);

                unsigned char leftmostCharWithCurrentLink = currentNode->checkLeftmostCharWithLink(key.data[currentCharIdx],
                                                                                                   *currentPointer);
                unsigned char rightmostCharWithCurrentLink = currentNode->checkRightmostCharWithLink(key.data[currentCharIdx],
                                                                                                     *currentPointer);

                unsigned char splitPoint = leaf->findBestSplitPoint(leftmostCharWithCurrentLink, rightmostCharWithCurrentLink);

                DatabaseKey dividingKey;
                dividingKey.data[0] = splitPoint;
                dividingKey.length = 1;

                leaf->moveAllBelowToAnotherLeaf(dividingKey, 0, *newLeaf);

                /* Current pointer might be overwritten while populating the new leaf below, and we need it a few lines later. */
                TriePointer currentPointerCopy = *currentPointer;

                if (newLeaf->isEmpty()) {
                    leaves->freeBin(newLeafId);
                    currentNode->setChildrenRange(leftmostCharWithCurrentLink, splitPoint - 1, TriePointer());
                } else {
                    currentNode->setChildrenRange(leftmostCharWithCurrentLink, splitPoint - 1, TriePointer(true, newLeafId));
                }

                if (leaf->isEmpty()) {
                    leaves->freeBin(currentPointerCopy.link);
                    currentNode->setChildrenRange(splitPoint, rightmostCharWithCurrentLink, TriePointer());
                } else {
                    currentNode->setChildrenRange(splitPoint, rightmostCharWithCurrentLink, currentPointerCopy);
                }
            }
        }
    }
}

template<typename ValueType>
void Trie<ValueType>::dump()
{
    int indendFactor = 4;
    int indent = indendFactor;
    string ind = "    ";
    queue<unsigned long long> nodesQueue;
    queue<unsigned long long> leavesQueue;
    char tmpValue[1024];

    nodesQueue.push(0);

    cout << "<trie>" << endl;

    while (! nodesQueue.empty()) {
        unsigned long long currentId = nodesQueue.front();
        nodesQueue.pop();
        TrieNode<ValueType> *currentNode = nodes->getBin(currentId);

        cout << ind << "<node id=\"" << currentId << "\">" << endl;

        for (int currentChar = 0x00; currentChar <= 0xff; currentChar++) {
            if (currentNode->values[currentChar] != 0) {
                cout << ind << ind << "<key char=\"" << currentChar
                     << "\" value=\"" << currentNode->values[currentChar] << "\" />" << endl;
            }
        }

        TriePointer *prevPointer = NULL;

        for (int currentCharIdx = 0x00; currentCharIdx <= 0xff; currentCharIdx++) {
            /* We can't use uchar in the loop, we would fall into infinite loop. */
            unsigned char currentChar = (unsigned char) currentCharIdx;

            TriePointer *currentPointer = &currentNode->children[currentChar];

            if ((currentCharIdx == 0x00) || (*prevPointer != *currentPointer)) {
                cout << ind << ind << "<from character=\"" << (int)currentChar;
                if (! currentPointer->isNull()) {
                    if (currentPointer->leaf == 1) {
                        leavesQueue.push(currentPointer->link);
                        cout << "\" type=\"leaf\" link=\"" << ((unsigned long long) currentPointer->link) << "\" />" << endl;
                    } else {
                        nodesQueue.push(currentPointer->link);
                        cout << "\" type=\"node\" link=\"" << ((unsigned long long) currentPointer->link) << "\" />" << endl;
                    }
                } else {
                    cout << "\" type=\"NULL\" link=\"NULL\" />" << endl;
                }

                prevPointer = currentPointer;
            }
        }

        cout << ind << "</node>" << endl;
    }

    while (! leavesQueue.empty()) {
        unsigned long long currentId = leavesQueue.front();
        leavesQueue.pop();
        TrieLeaf<ValueType> *currentLeaf = leaves->getBin(currentId);
        cout << ind << "<leaf id=\"" << currentId << "\">" << endl;

        for (TrieLeafNavigator<ValueType> navigator = currentLeaf->produceNaviagor(); !navigator.isEnd(); navigator.next()) {
            assert(navigator.getLength() < (sizeof(tmpValue) - 1));

            strncpy(tmpValue, (char*) navigator.getPointer(), navigator.getLength());
            tmpValue[navigator.getLength()] = 0x00;

            string tmpS(tmpValue);
            cout << ind << ind << "<item key=\"" << tmpS << "\" value=\"" << navigator.getValue() << "\" />" << endl;
        }

        cout << ind << "</leaf>" << endl;
    }

    cout << "</trie>" << endl;
}

template class Trie<unsigned long long>;

} /* namespace Db */
