/*
 * TrieMap.cpp
 *
 *  Created on: 06-12-2011
 *      Author: m
 */

#include <queue>
#include <string>
#include <iostream>
using namespace std;

#include "SystemParams.h"

#include "BinFileMap.h"

namespace Db {

BinFileMap::BinFileMap(const std::string &filename) {
    openMMapedFile(filename, SystemParams::initialIndexMapSize());
    cout << "Opened index map, loading" << endl;
    loadMapCache();
}

BinFileMap::~BinFileMap() {
    closeMMapedFile();
}

void BinFileMap::loadMapCache() {
    unsigned char *fileEnd = ((unsigned char*)fileStart) + mmaped_size;
    unsigned char *loc = (unsigned char*)fileStart;
    for (unsigned int i = 0;
         loc < fileEnd;
         loc++, i++)
    {
        for (int j = 0; j < 8; j++) {
            if ((*loc & (1 << j)) == 0) {
                /* We've got an empty bin for trie node */
                emptyBins.push(i * 8 + j);
                cout << "Pushing empty loc " << (i * 8 + j) << endl;
            }
        }
    }
}

unsigned long BinFileMap::fetchEmptyBin() {

    if (emptyBins.empty()) {
        return -1;
    }

    unsigned long result = emptyBins.front();
    emptyBins.pop();
    *(((unsigned char*)fileStart) + (result / 8)) |= (1 << (result % 8));
    return result;
}

void BinFileMap::makeBinEmpty(unsigned long index) {
    *(((unsigned char*)fileStart) + (index / 8)) &= ~(1 << (index % 8));
    emptyBins.push(index);
}

} /* namespace Db */