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
        cerr << __FILE__ << ":" << __LINE__ << " Couldn't fetch new leaf" << endl;\
        return;
    }

    rootNode->setChildrenRange(0x00, 0xff, TriePointer(true, leafID));
}

} /* namespace Db */
