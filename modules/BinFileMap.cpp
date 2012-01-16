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

#define BIN_FILE_MAP_EXPAND_SIZE SystemParams::pageSize()

namespace Db {

BinFileMap::BinFileMap(const std::string &filename)
    : endIterator(Iterator(*this))
{
    //openMMapedFile(filename, SystemParams::initialIndexMapSize());
    openMMapedFile(filename, BIN_FILE_MAP_EXPAND_SIZE);
    cerr << "Opened index map, loading" << endl;
    loadMapCache();
}

BinFileMap::~BinFileMap() {
    closeMMapedFile();
}

void BinFileMap::loadMapCache() {
    /* These nested loops iterate over bits in the map,
     * each iteration check bit numbered ((i * 8) + j).
     */
    for (off_t i = 0; i < mmaped_size; i++) {
        unsigned char *loc = (unsigned char*) getOffsetLoc(i);
        for (int j = 0; j < 8; j++) {
            if ((*loc & (1 << j)) == 0) {
                /* We've got an empty bin for trie node */
                emptyBins.push(i * 8 + j);
            }
        }
    }
}

unsigned long BinFileMap::fetchEmptyBin() {

    if (emptyBins.empty()) {
        cerr << "fetchEmptyBin: out of bins, expanding" << endl;

        size_t currentMmapedSize = mmaped_size;
        size_t newMmapedSize = mmaped_size + BIN_FILE_MAP_EXPAND_SIZE;
        extendFileAndMmapingToSize(newMmapedSize);

        for (int i = currentMmapedSize * 8; i < newMmapedSize * 8; i++) {
            emptyBins.push(i);
            cerr << "pusing " << i << endl;
        }
    }

    unsigned long result = emptyBins.front();
    emptyBins.pop();

    /* We've got the number of the bit we want, but we still need to extract
     * it from the memory.
     */
    *getOffsetLoc(result / 8) |= (1 << (result % 8));

    cerr << this << ": new empty bin " << result << endl;

    return result;
}

void BinFileMap::makeBinEmpty(unsigned long index) {
    *getOffsetLoc(index / 8) &= ~(1 << (index % 8));
    emptyBins.push(index);
}

BinFileMap::Iterator::Iterator(const BinFileMap &parent)
        : parent(parent), binId(0)
{
}

BinFileMap::Iterator& BinFileMap::Iterator::operator++()
{
    ++binId;
    return *this;
}

bool BinFileMap::Iterator::operator==(const Iterator &rhs)
{
    return binId == rhs.binId;
}

bool BinFileMap::Iterator::operator!=(const Iterator &rhs)
{
    return binId != rhs.binId;
}

std::pair<unsigned long, bool> BinFileMap::Iterator::operator*()
{
    unsigned char *loc = (unsigned char*) parent.getOffsetLoc(binId / 8);
    return std::pair<unsigned long, bool>(binId, (*loc & (1 << (binId % 8))) != 0);
}

BinFileMap::Iterator BinFileMap::getIterator() const
{
    return BinFileMap::Iterator(*this);
}

BinFileMap::Iterator& BinFileMap::end() const
{
    endIterator.binId = mmaped_size * 8;
    return endIterator;
}


} /* namespace Db */
