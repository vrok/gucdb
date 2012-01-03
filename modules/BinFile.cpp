/*
 * BinFile.cpp
 *
 *  Created on: 06-12-2011
 *      Author: m
 */

#include <cassert>

#include <iostream>
#include <string>
#include <cstring>
using namespace std;

#include "SystemParams.h"
#include "TrieNode.h"
#include "TrieLeaf.h"
#include "MMapedFile.h"

#include "BinFile.h"

namespace Db {

template<typename BinType>
BinFile<BinType>::BinFile(const string &filename, BinFileMap *trieMap, unsigned long initialFileSize)
: filename(filename), initialFileSize(initialFileSize), trieMap(trieMap) {

}

template<typename BinType>
BinFile<BinType>::~BinFile() {

}

template<typename BinType>
MMapedFile::OpeningResult BinFile<BinType>::openMMapedFile() {
    return MMapedFile::openMMapedFile(filename, initialFileSize);
}

template<typename BinType>
void BinFile<BinType>::assureNewBinIsUsable(off_t binOffset) {
    if (binOffset >= mmaped_size) {
        extendFileAndMmapingToSize(mmaped_size + initialFileSize);
        assert(binOffset < mmaped_size);
    }
}

template <typename BinType>
off_t BinFile<BinType>::getBinOffset(unsigned long id) {
    unsigned long binsPerExpandSize = minimalIndexExpandSize() / sizeof(BinType);

    unsigned long expandSizeLocation = minimalIndexExpandSize() * (id / binsPerExpandSize);

    return expandSizeLocation + (id % binsPerExpandSize) * sizeof(BinType);
}

template<typename BinType>
BinType *BinFile<BinType>::getBin(unsigned long id) {
    off_t binOffset = getBinOffset(id);
    return (BinType*) getOffsetLoc(binOffset);
}

template<typename BinType>
unsigned long long BinFile<BinType>::getNewBinByID() {
    unsigned long newBinId = trieMap->fetchEmptyBin();

    off_t binOffset = getBinOffset(newBinId);
    assureNewBinIsUsable(binOffset);

    BinType *bin = (BinType*) getOffsetLoc(binOffset);

    memset(bin, 0, sizeof(BinType));

    return newBinId;
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
template class BinFile<TrieLeaf>;

} /* namespace Db */
