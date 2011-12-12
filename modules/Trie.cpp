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

void Trie::addKey(const DatabaseKey &key, unsigned long long value) {
    TrieNode *currentNode = nodes->getBin(0); // root node always has id = 0

    int currentCharIdx = 0;

    TriePointer *currentPointer = &currentNode->children[key.data[currentCharIdx]];

    while (true) {

        bool isLinkPure = currentNode->isLinkPure(key.data[currentCharIdx]);

        currentCharIdx++;

        if (currentCharIdx == key.length) {
            /* The whole word had been eaten, and we haven't reached any leaf
             * node yet. We'll set the value pointer in the non-leaf node.
             */

            currentNode->value = value;
            return;
        }

        if (currentPointer->leaf == 0) {
            currentNode = nodes->getBin(currentPointer->link);
            currentPointer = &currentNode->children[key.data[currentCharIdx]];
        } else {
            TrieLeaf *leaf = leaves->getBin(currentPointer->link);

            while (true) {
                if (leaf->canFit(key, currentCharIdx)) {
                    leaf->add(key, currentCharIdx, value);
                    break;
                } else
                if (isLinkPure) {

                } else {
                    unsigned long long newLeafId = leaves->getNewBinByID();
                    TrieLeaf *newLeaf = leaves->getBin(newLeafId);

                    DatabaseKey dividingKey;
                    dividingKey.data[0] = key.data[currentCharIdx];
                    dividingKey.length = 1;

                    leaf->moveAllBelowToAnotherLeaf(key, 0, *newLeaf);

                    //currentNode->setChildrenRange(currentNode->checkLeftmostCharWithLink(currentCharIdx, currentPointer),
                    //                              currentPointer)
                }
            }
        }
    }

#if 0
    while ((currentPointer->leaf != 0) && (++currentCharIdx < key.length)) {

    }
#endif

}

} /* namespace Db */
