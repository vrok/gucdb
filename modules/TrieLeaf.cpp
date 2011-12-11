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

#define DATA_LOCATION_TO_UL(loc)    (*((unsigned long*)(loc)))
#define DATA_LOCATION_TO_ULL(loc)   (*((unsigned long long*)(loc)))
/* Used size is stored in the first location of the data array. */
#define LEAF_USED_SIZE              (DATA_LOCATION_TO_UL(data) + sizeof(unsigned long))
#define DATA_AFTER_LEAF_USED_SIZE   (data + sizeof(unsigned long))

unsigned char *TrieLeaf::find(const DatabaseKey &key, int firstCharactedIdx)
{
    unsigned char *currentLoc = DATA_AFTER_LEAF_USED_SIZE;

    while (currentLoc < (data + LEAF_USED_SIZE)) {
        unsigned char *endCharacter = currentLoc + sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc);
        const unsigned char *endKeyCharacter = key.data + key.length;
        unsigned char *currentCharacter = currentLoc + sizeof(unsigned long);
        const unsigned char *currentKeyCharacter = key.data + firstCharactedIdx;

        while ((currentCharacter < endCharacter) &&
               (currentKeyCharacter < endKeyCharacter) &&
               (*currentCharacter == *currentKeyCharacter))
        {

            currentCharacter++;
            currentKeyCharacter++;
        }

        if ((currentCharacter == endCharacter) && (currentKeyCharacter == endKeyCharacter)) {
            return currentLoc;
        }

        currentLoc += sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc) + sizeof(unsigned long long);
    }

    return NULL;
}

unsigned long long TrieLeaf::get(const DatabaseKey &key, int firstCharactedIdx)
{
    unsigned char *searchResult = find(key, firstCharactedIdx);
    if (searchResult == NULL) {
        return 0;
    }

    return DATA_LOCATION_TO_ULL(searchResult + sizeof(unsigned long) + DATA_LOCATION_TO_UL(searchResult));
}

void TrieLeaf::add(const DatabaseKey &key, int firstCharacterIdx, unsigned long long value)
{
    assert(canFit(key, firstCharacterIdx));

    unsigned long activeKeyLength = key.length - firstCharacterIdx;

    DATA_LOCATION_TO_UL(data + LEAF_USED_SIZE) = activeKeyLength;

    memcpy((void*)(data + LEAF_USED_SIZE + sizeof(unsigned long)),
           (void*)(key.data + firstCharacterIdx), activeKeyLength);

    DATA_LOCATION_TO_ULL(data + LEAF_USED_SIZE + sizeof(unsigned long) + activeKeyLength) = value;

    DATA_LOCATION_TO_UL(data) += sizeof(unsigned long) + activeKeyLength + sizeof(unsigned long long);
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
