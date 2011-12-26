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

#include <iostream>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

namespace Db {

Trie::Trie(BinFile<TrieNode> *nodes, BinFile<TrieLeaf> *leaves)
    : nodes(nodes), leaves(leaves) {

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

    cout << "Index leaves file opened" << endl;


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

    cout << "Index file opened" << endl;

}

Trie::~Trie() {
    nodes->closeMMapedFile();
    leaves->closeMMapedFile();
}

void Trie::initializeEmpty() {
    cout << "Initializing empty trie file" << endl;

    TrieNode *rootNode = nodes->getBin(0);

    unsigned long long leafID = leaves->getNewBinByID();

    if (leafID == -1) {
        cerr << __FILE__ << ":" << __LINE__ << " Couldn't fetch a new leaf" << endl;\
        return;
    }

    rootNode->setChildrenRange(0x00, 0xff, TriePointer(true, leafID));
}

unsigned long long Trie::get(const DatabaseKey &key) {
    int currentCharIdx = 0;

    TrieNode *currentNode = nodes->getBin(0);

    while (true) {
        TriePointer *currentPointer = &currentNode->children[key.data[currentCharIdx]];

        if (currentPointer->leaf == 0) {
            currentCharIdx++;
            currentNode = nodes->getBin(currentPointer->link);

            if (currentCharIdx == key.length) {
                return currentNode->value;
            }
        } else {
            TrieLeaf *leaf = leaves->getBin(currentPointer->link);
            return leaf->get(key, currentCharIdx);
        }
    }

    cerr << __FILE__ << ":" << __LINE__ << " Reached line that should not be reached" << endl;
    return 0;
}

void Trie::addKey(const DatabaseKey &key, unsigned long long value) {
    int currentCharIdx = 0;

    TrieNode *currentNode = nodes->getBin(0); // root node always has id = 0
    //TriePointer *currentPointer = &currentNode->children[key.data[currentCharIdx]];

    while (true) {
        bool isLinkPure = currentNode->isLinkPure(key.data[currentCharIdx]);
        TriePointer *currentPointer = &currentNode->children[key.data[currentCharIdx]];

        if (currentPointer->leaf == 0) {
            currentCharIdx++;
            currentNode = nodes->getBin(currentPointer->link);
            //currentPointer = &currentNode->children[key.data[currentCharIdx]];

            if (currentCharIdx == key.length) {
                /* The whole word had been eaten, and we haven't reached any leaf
                 * node yet. We'll set the value pointer in the non-leaf node.
                 */

                currentNode->value = value;
                return;
            }
        } else {
            TrieLeaf *leaf = leaves->getBin(currentPointer->link);

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
                TrieNode *newNode = nodes->getBin(newNodeIndex);
                currentPointer->leaf = 0;
                currentPointer->link = newNodeIndex;
                newNode->setChildrenRange(0, 255, newPointer);

                currentCharIdx++;
                currentNode = newNode;
            } else {
                unsigned long long newLeafId = leaves->getNewBinByID();
                TrieLeaf *newLeaf = leaves->getBin(newLeafId);

                unsigned char splitPoint = leaf->findBestSplitPoint();

                DatabaseKey dividingKey;
                dividingKey.data[0] = key.data[splitPoint];
                dividingKey.length = 1;

                leaf->moveAllBelowToAnotherLeaf(key, 0, *newLeaf);

                unsigned char leftmostCharWithCurrentLink = currentNode->checkLeftmostCharWithLink(splitPoint, *currentPointer);
                unsigned char rightmostCharWithCurrentLink = currentNode->checkRightmostCharWithLink(splitPoint, *currentPointer);

                if (newLeaf->isEmpty()) {
                    leaves->freeBin(newLeafId);
                    currentNode->setChildrenRange(leftmostCharWithCurrentLink, splitPoint - 1, TriePointer());
                } else {
                    currentNode->setChildrenRange(leftmostCharWithCurrentLink, splitPoint - 1, *currentPointer);
                }

                if (leaf->isEmpty()) {
                    leaf->add(key, currentCharIdx, value);
                    return;
                    //leaves->freeBin(currentPointer->link);
                    //currentNode->setChildrenRange(splitPoint, rightmostCharWithCurrentLink, TriePointer());
                } else {
                    currentNode->setChildrenRange(splitPoint, rightmostCharWithCurrentLink, TriePointer(true, newLeafId));
                }
            }
        }
    }
}

} /* namespace Db */
