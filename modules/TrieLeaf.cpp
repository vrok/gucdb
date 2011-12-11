/*
 * TrieLeaf.cpp
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#include "TrieLeaf.h"

#include <cassert>
#include <cstring>
#include <iostream>

namespace Db {

#define DATA_LOCATION_TO_UL(loc)    (*((unsigned long*)(loc)))
#define DATA_LOCATION_TO_ULL(loc)   (*((unsigned long long*)(loc)))
/* Used size is stored in the first location of the data array. */
#define LEAF_USED_SIZE              (DATA_LOCATION_TO_UL(data) + sizeof(unsigned long))
#define DATA_AFTER_LEAF_USED_SIZE   (data + sizeof(unsigned long))

unsigned char *TrieLeaf::find(const DatabaseKey &key, int firstCharacterIdx)
{
    unsigned char *currentLoc = DATA_AFTER_LEAF_USED_SIZE;

    while (currentLoc < (data + LEAF_USED_SIZE)) {
        unsigned char *endCharacter = currentLoc + sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc);
        const unsigned char *endKeyCharacter = key.data + key.length;
        unsigned char *currentCharacter = currentLoc + sizeof(unsigned long);
        const unsigned char *currentKeyCharacter = key.data + firstCharacterIdx;

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

void TrieLeaf::addBulk(unsigned char *source, unsigned long length)
{
    memcpy(data + LEAF_USED_SIZE, source, length);
    DATA_LOCATION_TO_UL(data) += length;
}

unsigned long long TrieLeaf::get(const DatabaseKey &key, int firstCharacterIdx)
{
    unsigned char *searchResult = find(key, firstCharacterIdx);
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
    unsigned char *searchResult = find(key, firstCharacterIdx);
    if (searchResult == NULL) {
        return;
    }

    unsigned long deletedSlotSize = sizeof(unsigned long) + DATA_LOCATION_TO_UL(searchResult) + sizeof(unsigned long long);
    unsigned char *currentLoc = searchResult + deletedSlotSize;

    while (currentLoc < (data + LEAF_USED_SIZE)) {
        unsigned long currentSlotSize = sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc) + sizeof(unsigned long long);
        memmove(currentLoc - deletedSlotSize, currentLoc, currentSlotSize);
        currentLoc += currentSlotSize;
    }

    DATA_LOCATION_TO_UL(data) -= deletedSlotSize;
}

bool TrieLeaf::canFit(const DatabaseKey &key, int firstCharacterIdx)
{
    return (key.length - firstCharacterIdx) <= (sizeof(data) - LEAF_USED_SIZE);
}

void TrieLeaf::moveAllBelowToAnotherLeaf(const DatabaseKey &key, int firstCharacterIdx, TrieLeaf &anotherLeaf)
{
    unsigned char *currentLoc = DATA_AFTER_LEAF_USED_SIZE;
    unsigned long shift = 0;

    while (currentLoc < (data + LEAF_USED_SIZE)) {
        unsigned long currentSlotSize = sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc) + sizeof(unsigned long long);

        unsigned char *endCharacter = currentLoc + sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc);
        const unsigned char *endKeyCharacter = key.data + key.length;
        unsigned char *currentCharacter = currentLoc + sizeof(unsigned long);
        const unsigned char *currentKeyCharacter = key.data + firstCharacterIdx;

        while ((currentCharacter < endCharacter) &&
               (currentKeyCharacter < endKeyCharacter) &&
               (*currentCharacter == *currentKeyCharacter))
        {

            currentCharacter++;
            currentKeyCharacter++;
        }

        int strCompare = 0;

        if ((currentCharacter == endCharacter) && (currentKeyCharacter == endKeyCharacter)) {
            strCompare = 0;
        } else
        if (currentCharacter == endCharacter) {
            strCompare = -1;
        } else
        if (currentKeyCharacter == endKeyCharacter) {
            strCompare = 1;
        } else {
            strCompare = *currentCharacter > *currentKeyCharacter ? -1 : 1;
        }

        if (strCompare > 0) {
            anotherLeaf.addBulk(currentLoc, currentSlotSize);
            shift += currentSlotSize;
        } else {
            if (shift > 0) {
                memmove(currentLoc - shift, currentLoc, currentSlotSize);
            }
        }

        currentLoc += currentSlotSize;
    }

    DATA_LOCATION_TO_UL(data) -= shift;
}

}

/* namespace Db */
