/*
 * TrieLeaf.cpp
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#include "TrieLeaf.h"

#include <cassert>
#include <cstring>

namespace Db {

#define DATA_LOCATION_TO_ULL(loc)   (*((unsigned long*)loc))
/* Used size is stored in the first location of the data array. */
#define LEAF_USED_SIZE              DATA_LOCATION_TO_ULL(data)
#define DATA_AFTER_LEAF_USED_SIZE   DATA_LOCATION_TO_ULL(data + sizeof(unsigned long))

unsigned long long TrieLeaf::get(const DatabaseKey &key, int firstCharactedIdx)
{
    return 0;
}

void TrieLeaf::add(const DatabaseKey &key, int firstCharacterIdx, unsigned long long value)
{
    assert(canFit(key, firstCharacterIdx));

    int activeKeyLength = key.length - firstCharacterIdx;

    DATA_LOCATION_TO_ULL(data + LEAF_USED_SIZE) = activeKeyLength;

    memcpy((void*)(data + LEAF_USED_SIZE + sizeof(unsigned long)),
           (void*)(key.data + firstCharacterIdx), activeKeyLength);

    LEAF_USED_SIZE += sizeof(unsigned long) + activeKeyLength;
}

void TrieLeaf::remove(const DatabaseKey &key, int firstCharacterIdx)
{
}

bool TrieLeaf::canFit(const DatabaseKey &key, int firstCharacterIdx)
{
    return (key.length - firstCharacterIdx) <= (sizeof(data) - LEAF_USED_SIZE);
}

void TrieLeaf::moveAllBelowToAnotherLeaf(const DatabaseKey &key, int firstCharacterIdx, TrieLeaf &anotherLeaf)
{
}

void TrieLeaf::vacuum()
{

}

}

/* namespace Db */
