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

#define DATA_LOCATION_TO_UL(loc)         (*((unsigned long*)(loc)))
#define DATA_LOCATION_TO_ULL(loc)        (*((unsigned long long*)(loc)))
/* Used size is stored in the first location of the data array. */
#define LEAF_USED_SIZE                   (DATA_LOCATION_TO_UL(data) + sizeof(unsigned long))
#define OTHER_LEAF_USED_SIZE(leaf_ptr)   (DATA_LOCATION_TO_UL(leaf_ptr->data) + sizeof(unsigned long))
#define DATA_AFTER_LEAF_USED_SIZE        (data + sizeof(unsigned long))

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

bool TrieLeaf::isEmpty()
{
    /* Empty node only stores its size in first few bytes. */
    return LEAF_USED_SIZE == sizeof(unsigned long);
}

bool TrieLeaf::canFit(const DatabaseKey &key, int firstCharacterIdx)
{
    return (key.length - firstCharacterIdx + sizeof(unsigned int) + sizeof(unsigned long long)) <= (sizeof(data) - LEAF_USED_SIZE);
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

void TrieLeaf::moveAllEqualOrBiggerToAnotherLeaf(unsigned char initialCharacter)
{
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

    for (int i = 0; i < sizeof(sizesPerFirstCharacter) / sizeof(unsigned long); i++) {
        /* We loop with int and then cast, to avoid uchar overflow causing infinite loop */
        if ((sizesPerFirstCharacter[i] + currentAccumulatedSizes) >= (leafWithoutHeaderSize / 2)) {
            if (currentAccumulatedSizes > 0) {
                return (unsigned char) i;
            } else {
                /* First character with more than zero occurrences happens to overwhelm the leaf node.
                 * Nested loop below checks if there's any word left starting with a different char,
                 * depending on that, we return the current index, or the next one.
                 */
                for (int j = i + 1; j < sizeof(sizesPerFirstCharacter) / sizeof(unsigned long); j++) {
                    if (sizesPerFirstCharacter[j] > 0) {
                        return (unsigned char) (i + 1);
                    }
                }
                return (unsigned char) i;
            }
        }

        currentAccumulatedSizes += sizesPerFirstCharacter[i];
    }

    /* It should never end up here. */
    return sizeof(unsigned int) - 1;
}

unsigned long long TrieLeaf::stripLeadingCharacter()
{
    unsigned char *currentLoc = DATA_AFTER_LEAF_USED_SIZE;
    unsigned long long result = 0;
    unsigned long shift = 0;

    while (currentLoc < (data + LEAF_USED_SIZE)) {
        unsigned long currentKeyLen = DATA_LOCATION_TO_UL(currentLoc);
        unsigned long currentSlotSize = sizeof(unsigned long) + currentKeyLen + sizeof(unsigned long long);

        if (DATA_LOCATION_TO_UL(currentLoc) == 1) {
            /* Key ends exactly in the leaf. We are expanding vertically, thus this value should
             * be moved upwards, to the newly created node.
             */
            result = DATA_LOCATION_TO_ULL(currentLoc + sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc));
            shift += currentSlotSize;
            currentLoc += currentSlotSize;
            continue;
        }

        DATA_LOCATION_TO_UL(currentLoc - shift) = currentKeyLen - 1;
        memmove(currentLoc + sizeof(unsigned long) - shift,
                currentLoc + sizeof(unsigned long) + 1,
                currentKeyLen - 1 + sizeof(unsigned long long));

        shift++;
        currentLoc += currentSlotSize;
    }

    DATA_LOCATION_TO_UL(data) -= shift;
    return result;
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

TrieLeafNavigator TrieLeaf::produceNaviagor()
{
    return TrieLeafNavigator(DATA_AFTER_LEAF_USED_SIZE, this);
}


TrieLeafNavigator::TrieLeafNavigator(unsigned char *currentLoc, TrieLeaf *context)
        : currentLoc(currentLoc), context(context) {
}

unsigned char* TrieLeafNavigator::getPointer() {
    return currentLoc + sizeof(unsigned long);
}

unsigned int TrieLeafNavigator::getLength() {
    return DATA_LOCATION_TO_UL(currentLoc);
}

unsigned long long TrieLeafNavigator::getValue() {
    return DATA_LOCATION_TO_ULL(currentLoc + sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc));
}

void TrieLeafNavigator::next() {
    currentLoc += sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc) + sizeof(unsigned long long);
}

bool TrieLeafNavigator::isEnd() {
    return currentLoc >= (context->data + OTHER_LEAF_USED_SIZE(context));
}


}

/* namespace Db */
