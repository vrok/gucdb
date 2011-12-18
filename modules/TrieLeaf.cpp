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

        int strCompare = compareKeys(currentLoc + sizeof(unsigned long),
                                     currentLoc + sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc),
                                     key,
                                     firstCharacterIdx);

        if (strCompare == 0) {
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

int TrieLeaf::compareKeys(unsigned char *currentCharacter, unsigned char *endCharacter, const DatabaseKey &key, int firstCharacterIdx)
{
    const unsigned char *endKeyCharacter = key.data + key.length;
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
        return 0;
    } else
    if (currentCharacter == endCharacter) {
        return 1;
    } else
    if (currentKeyCharacter == endKeyCharacter) {
        return -1;
    } else {
        return *currentCharacter > *currentKeyCharacter ? -1 : 1;
    }
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

        int strCompare = compareKeys(currentLoc + sizeof(unsigned long),
                                     currentLoc + sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc),
                                     key,
                                     firstCharacterIdx);

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

unsigned char TrieLeaf::findBestSplitPoint()
{
    unsigned char *currentLoc = DATA_AFTER_LEAF_USED_SIZE;
    unsigned long sizesPerFirstCharacter[256];
    memset(sizesPerFirstCharacter, 0, sizeof(sizesPerFirstCharacter));

    while (currentLoc < (data + LEAF_USED_SIZE)) {
        unsigned long currentSlotSize = sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc) + sizeof(unsigned long long);

        assert(DATA_LOCATION_TO_UL(currentLoc) != 0); /* We can't get first character of an empty string. */

        sizesPerFirstCharacter[*(currentLoc + sizeof(unsigned long))] += currentSlotSize;

        currentLoc += currentSlotSize;
    }

    unsigned long currentAccumulatedSizes = 0;
    unsigned long leafWithoutHeaderSize = LEAF_USED_SIZE - sizeof(unsigned long);

    for (unsigned char i = 0; i < sizeof(sizesPerFirstCharacter); i++) {
        if ((sizesPerFirstCharacter[i] + currentAccumulatedSizes) >= (leafWithoutHeaderSize / 2)) {
            if (currentAccumulatedSizes > 0) {
                return i - 1;
            } else {
                return i;
            }
        }

        currentAccumulatedSizes += sizesPerFirstCharacter[i];
    }

    /* It should never end up here. */
    return sizeof(unsigned int) - 1;
}

void TrieLeaf::divideIntoTwoBasedOnFirstChar(unsigned char comparator, TrieLeaf &anotherLeaf)
{
#if 0
    unsigned char *currentLoc = DATA_AFTER_LEAF_USED_SIZE;
    unsigned long shift = 0;

    while (currentLoc < (data + LEAF_USED_SIZE)) {
        unsigned long currentSlotSize = sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc) + sizeof(unsigned long long);

        if ((DATA_LOCATION_TO_UL(currentLoc) == 0
    }
#endif
    }

    }

/* namespace Db */
