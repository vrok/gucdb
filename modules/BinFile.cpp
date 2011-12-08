/*
 * BinFile.cpp
 *
 *  Created on: 06-12-2011
 *      Author: m
 */

#include <string>
using namespace std;

#include "SystemParams.h"
#include "TrieNode.h"

#include "BinFile.h"

namespace Db {

template<typename BinType>
BinFile<BinType>::BinFile(const string &filename, BinFileMap *trieMap, unsigned long initialFileSize)
: initialFileSize(initialFileSize), trieMap(trieMap) {
    openMMapedFile(filename, initialFileSize);
}

template<typename BinType>
BinFile<BinType>::~BinFile() {
    closeMMapedFile();
}

template<typename BinType>
BinType *BinFile<BinType>::getBin(unsigned long int id) {
    unsigned long binsPerExpandSize = minimalIndexExpandSize() / sizeof(BinType);

    unsigned long expandSizeLocation = minimalIndexExpandSize() * (id / binsPerExpandSize);

    return (BinType*) (((unsigned char*) fileStart) + expandSizeLocation + (id % binsPerExpandSize) * sizeof(BinType));
}

template<typename BinType>
BinType *BinFile<BinType>::getNewBin() {
    unsigned long id = trieMap->fetchEmptyBin();
    if (id == -1) {
        return (BinType*) NULL;
    }
    return getBin(id);
}

template<typename BinType>
void BinFile<BinType>::freeBin(unsigned long int id) {
    trieMap->makeBinEmpty(id);
}

/* TODO: code below could be easily computed during compilation in c++11 */
template<typename BinType>
unsigned long BinFile<BinType>::minimalIndexExpandSize() {
    unsigned long pSize = SystemParams::pageSize();

    if (pSize < sizeof(BinType)) {
        if (sizeof(BinType) % pSize == 0) {
            return sizeof(BinType);
        } else {
            return pSize * ((sizeof(BinType) / pSize) + 1);
        }
    } else {
        return pSize;
    }
}

template class BinFile<TrieNode>;

} /* namespace Db */
