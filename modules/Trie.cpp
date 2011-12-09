/*
 * Trie.cpp
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#include "SystemParams.h"
#include "TrieNode.h"
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

Trie::Trie(const string & filename, BinFileMap *trieMap)
: BinFile<TrieNode>::BinFile(filename, trieMap, SystemParams::initialIndexSize()) {

    switch (openMMapedFile(filename, initialFileSize)) {
    case OPENED:
        break;
    case NEW_FILE:
        initializeEmpty();
        break;
    case ERROR:
    default:
        cerr << "Opening trie file failed" << endl;
        break;
    }

    //openMMapedFile(filename, SystemParams::initialIndexSize());
    cout << "Index file opened" << endl;

    TrieNode *rootNode = getBin(0);
}

Trie::~Trie() {
    closeMMapedFile();
}

void Trie::initializeEmpty() {
    cout << "Initializing empty trie file" << endl;

    TrieNode *rootNode = getBin(0);
}

} /* namespace Db */
