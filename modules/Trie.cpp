/*
    Copyright 2013 Marcin Wrochniak

    This file is part of Guc DB.

    Guc DB is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    Guc DB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * Trie.cpp
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#include "SystemParams.h"
#include "BinFile.h"
#include "BinFileMap.h"
#include "TrieNode.h"
#include "MMapedFile.h"
#include "Slabs.h"

#include "Trie.h"


#include <cstring>
#include <cassert>
#include <iostream>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
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

    rootNode->setChildrenRange(*nodes, 0x00, 0xff, TriePointer(true, leafID));
}

template<typename ValueType>
ValueType Trie<ValueType>::get(const DatabaseKey &key)
{
    int currentCharIdx = 0;

    TrieNode<ValueType> *currentNode = nodes->getBin(0);

    while (true) {
        TriePointer *currentPointer = &currentNode->getChildPointer(*nodes, key[currentCharIdx]);

        if (currentPointer->isNull()) {
            return 0;
        } else
        if (currentPointer->leaf == 0) {
            currentCharIdx++;
            currentNode = nodes->getBin(currentPointer->link);

            if (currentCharIdx == key.getLength()) {
                return currentNode->getValue(*nodes, key[currentCharIdx - 1]);
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
    TrieNode<ValueType> *currentNode = nodes->getBin(0); /* Root node always has id = 0 */

    while (true) {
        bool isLinkPure = currentNode->isLinkPure(*nodes, key[currentCharIdx]);
        TriePointer *currentPointer = &currentNode->getChildPointer(*nodes, key[currentCharIdx]);

        if (currentPointer->isNull()) {
            unsigned char leftmostCharWithCurrentLink = currentNode->checkLeftmostCharWithLink(*nodes, key[currentCharIdx],
                                                                                               *currentPointer);
            unsigned char rightmostCharWithCurrentLink = currentNode->checkRightmostCharWithLink(*nodes, key[currentCharIdx],
                                                                                                 *currentPointer);

            unsigned long long newLeafId = leaves->getNewBinByID();
            TrieLeaf<ValueType> *newLeaf = leaves->getBin(newLeafId);

            currentNode->setChildrenRange(*nodes, leftmostCharWithCurrentLink, rightmostCharWithCurrentLink,
                                          TriePointer(true, newLeafId));

            newLeaf->add(key, currentCharIdx, value);
            return;
        } else
        if (currentPointer->leaf == 0) {
            currentCharIdx++;
            currentNode = nodes->getBin(currentPointer->link);

            if (currentCharIdx == key.getLength()) {
                /* The whole word had been eaten, and we haven't reached any leaf
                 * node yet. We'll set the value pointer in the non-leaf node.
                 */

                currentNode->setValue(*nodes, key[currentCharIdx - 1], value);
                return;
            }
        } else {
            TrieLeaf<ValueType> *leaf = leaves->getBin(currentPointer->link);

            if (leaf->find(key, currentCharIdx) != NULL) {
                leaf->update(key, currentCharIdx, value);
                return;
            } else
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
                newNode->setValue(*nodes, key[currentCharIdx], newNodeValue);
                newNode->setChildrenRange(*nodes, 0x00, 0xff, newPointer);

                currentCharIdx++;
                currentNode = newNode;

                if (currentCharIdx == key.getLength()) {
                    /* It might happen that the word ends exactly at the fresh node
                     * TODO: we overwrite value
                     */
                    currentNode->setValue(*nodes, key[currentCharIdx - 1], value);
                    return;
                }
            } else {
                unsigned long long newLeafId = leaves->getNewBinByID();
                TrieLeaf<ValueType> *newLeaf = leaves->getBin(newLeafId);

                unsigned char leftmostCharWithCurrentLink = currentNode->checkLeftmostCharWithLink(*nodes, key[currentCharIdx],
                                                                                                   *currentPointer);
                unsigned char rightmostCharWithCurrentLink = currentNode->checkRightmostCharWithLink(*nodes, key[currentCharIdx],
                                                                                                     *currentPointer);

                unsigned char splitPoint = leaf->findBestSplitPoint(leftmostCharWithCurrentLink, rightmostCharWithCurrentLink);

                DatabaseKey dividingKey;
                dividingKey.fillWithOneChar(splitPoint);

                leaf->moveAllBelowToAnotherLeaf(dividingKey, 0, *newLeaf);

                /* Current pointer might be overwritten while populating the new leaf below, and we need it a few lines later. */
                TriePointer currentPointerCopy = *currentPointer;

                if (newLeaf->isEmpty()) {
                    leaves->freeBin(newLeafId);
                    currentNode->setChildrenRange(*nodes, leftmostCharWithCurrentLink, splitPoint - 1, TriePointer());
                } else {
                    currentNode->setChildrenRange(*nodes, leftmostCharWithCurrentLink, splitPoint - 1, TriePointer(true, newLeafId));
                }

                if (leaf->isEmpty()) {
                    leaves->freeBin(currentPointerCopy.link);
                    currentNode->setChildrenRange(*nodes, splitPoint, rightmostCharWithCurrentLink, TriePointer());
                } else {
                    currentNode->setChildrenRange(*nodes, splitPoint, rightmostCharWithCurrentLink, currentPointerCopy);
                }
            }
        }
    }
}

template <typename ValueType>
bool Trie<ValueType>::deleteKeyAndJustDecideIfShouldCleanUpTrie(const DatabaseKey &key, vector<TrieNode<ValueType>*> &path)
{
    int currentCharIdx = 0;

    TrieNode<ValueType> *currentNode = nodes->getBin(0);
    path.push_back(currentNode);
    TriePointer *currentPointer;

    while (currentCharIdx < key.getLength()) {
        currentPointer = &currentNode->getChildPointer(*nodes, key[currentCharIdx]);

        if (currentPointer->leaf == 0) {
            currentCharIdx++;
            currentNode = nodes->getBin(currentPointer->link);

            if (currentCharIdx == key.getLength()) {
                currentNode->setValue(*nodes, key[currentCharIdx - 1], 0);

                if (! currentNode->isPointerTheOnlyNonNullField(*nodes, TriePointer())) {
                    return false;
                }
            } else {
                path.push_back(currentNode);
            }
        } else {
            TrieLeaf<ValueType> *leaf = leaves->getBin(currentPointer->link);
            leaf->remove(key, currentCharIdx);

            if (! leaf->isEmpty()) {
                return false;
            }

            break;
        }
    }

    return true;
}

template <typename ValueType>
void Trie<ValueType>::cleanUpTrieAfterKeyRemoval(const DatabaseKey &key, const vector<TrieNode<ValueType>*> &pathToRemoved)
{
    int nodeCount = pathToRemoved.size();

    for (int i = nodeCount - 1; i >= 0; --i) {
        TriePointer &currentLink = pathToRemoved[i]->getChildPointer(*nodes, key[i]);
        bool shouldContinue = pathToRemoved[i]->isPointerTheOnlyNonNullField(*nodes, currentLink);

        if (shouldContinue) {
            if (currentLink.leaf == 1) {
                leaves->freeBin(currentLink.link);
            } else {
                nodes->freeBin(currentLink.link);
            }

            pathToRemoved[i]->setChildrenRange(*nodes, 0x00, 0xff, TriePointer());
        } else {
            unsigned char leftmostCharWithCurrentLink = pathToRemoved[i]->checkLeftmostCharWithLink(*nodes, key[i], currentLink);
            unsigned char rightmostCharWithCurrentLink = pathToRemoved[i]->checkRightmostCharWithLink(*nodes, key[i], currentLink);

            /* We could just fill pointers from the leftmost to the rightmost matched char with zeros,
             * but we want to maximally fill the leaves we have, so try to merge the pointers with
             * their neighbours (provided that these neighbours are linked to leafs (i.e. they are not pure)).
             */
            if ((leftmostCharWithCurrentLink > 0) && !pathToRemoved[i]->getChildPointer(*nodes, leftmostCharWithCurrentLink - 1).isNull()
                    && !pathToRemoved[i]->isLinkPure(*nodes, leftmostCharWithCurrentLink - 1)) {

                pathToRemoved[i]->setChildrenRange(*nodes, leftmostCharWithCurrentLink, rightmostCharWithCurrentLink,
                                          pathToRemoved[i]->getChildPointer(*nodes, leftmostCharWithCurrentLink - 1));
            } else
            if ((rightmostCharWithCurrentLink < 0xff) && !pathToRemoved[i]->getChildPointer(*nodes, rightmostCharWithCurrentLink + 1).isNull()
                    && !pathToRemoved[i]->isLinkPure(*nodes, rightmostCharWithCurrentLink + 1)) {

                pathToRemoved[i]->setChildrenRange(*nodes, leftmostCharWithCurrentLink, rightmostCharWithCurrentLink,
                                          pathToRemoved[i]->getChildPointer(*nodes, rightmostCharWithCurrentLink + 1));
            } else {
                pathToRemoved[i]->setChildrenRange(*nodes, leftmostCharWithCurrentLink, rightmostCharWithCurrentLink, TriePointer());
            }

            break;
        }
    }
}

template<typename ValueType>
void Trie<ValueType>::deleteKey(const DatabaseKey &key)
{
    /* We need to remember the nodes we visit, because we need them in a later phase.
     * In case this turns out to be slow (it uses heap), one alternative
     * that might be viable is doing this deletion recursively (we could
     * just use stack then (but what about stack overflows?)).
     */
    vector<TrieNode<ValueType>*> path;
    path.reserve(10);

    if (! deleteKeyAndJustDecideIfShouldCleanUpTrie(key, path))
        return;

    cleanUpTrieAfterKeyRemoval(key, path);
}

template<typename ValueType>
void Trie<ValueType>::dump()
{
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
            if (currentNode->getValue(*nodes, currentChar) != 0) {
                cout << ind << ind << "<key char=\"" << currentChar
                     << "\" value=\"" << currentNode->getValue(*nodes, currentChar) << "\" />" << endl;
            }
        }

        TriePointer *prevPointer = NULL;

        for (int currentCharIdx = 0x00; currentCharIdx <= 0xff; currentCharIdx++) {
            /* We can't use uchar in the loop, we would fall into infinite loop. */
            unsigned char currentChar = (unsigned char) currentCharIdx;

            TriePointer *currentPointer = &currentNode->getChildPointer(*nodes, currentChar);

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

//template class Trie<unsigned long long>;
template class Trie<ObjectID>;

} /* namespace Db */
