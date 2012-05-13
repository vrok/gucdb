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

#include "Slabs.h"
#include "Trie.h"
#include "Database.h"
#include "SystemParams.h"
#include "TrieNode.h"
#include "TrieLeaf.h"
#include "MMapedFile.h"

#include "BinFile.h"

namespace Db {

template<typename BinType>
BinFile<BinType>::BinFile(const string &filename, BinFileMap *trieMap, unsigned long initialFileSize)
    : filename(filename), initialFileSize(initialFileSize), binFileMap(trieMap)
{
    /* These asserts are for future developers who would try to store not nicely padded objects
     * using this class (it would introduce nasty, intricate bugs).
     */
    if (sizeof(BinType) > SystemParams::pageSize()) {
        assert(0 == (sizeof(BinType) % SystemParams::pageSize()));
    } else {
        assert(0 == (SystemParams::pageSize() % sizeof(BinType)));
    }
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
    if ((binOffset + sizeof(BinType)) >= mmaped_size) {
        //cerr << this << ": Bin not in range, expanding " << binOffset << endl;
        extendFileAndMmapingToSize(mmaped_size + initialFileSize);
        assert((binOffset + sizeof(BinType)) < mmaped_size);
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
    unsigned long newBinId = binFileMap->fetchEmptyBin();

    off_t binOffset = getBinOffset(newBinId);
    assureNewBinIsUsable(binOffset);

    BinType *bin = (BinType*) getOffsetLoc(binOffset);

    memset(bin, 0, sizeof(BinType));

    return newBinId;
}

template<typename BinType>
bool BinFile<BinType>::isBinFree(unsigned long id)
{
    return binFileMap->isBinEmpty(id);
}

template<typename BinType>
bool BinFile<BinType>::isBinIDSafeAndAllocated(unsigned long id)
{
    /* We check:
     * - If BinType object can be fit at given location
     *   (it's not enough to be before the end of mmaped space,
     *   sizeof(BinType) bytes still need to be left)
     * - If it's actually used (not free).
     */
    return ((id + 1) * sizeof(BinType) <= mmaped_size)
           && !isBinFree(id);
}

template<typename BinType>
void BinFile<BinType>::freeBin(unsigned long int id) {
    binFileMap->makeBinEmpty(id);
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

template<typename BinType>
const BinFileMap & BinFile<BinType>::getBinFileMap() {
    return *binFileMap;
}

template class BinFile<TrieNode<ObjectID> >;
template class BinFile<TrieLeaf<ObjectID> >;
template class BinFile<SlabInfo>;
template class BinFile<Slab>;

} /* namespace Db */
