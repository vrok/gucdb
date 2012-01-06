/*
 * TrieLeaf.cpp
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#include "Trie.h"

#include "TrieLeaf.h"

#include <cassert>
#include <cstring>
#include <iostream>
using namespace std;

namespace Db {

#define DATA_LOCATION_TO_UL(loc)         (*((unsigned long*)(loc)))
#define DATA_LOCATION_TO_ULL(loc)        (*((unsigned long long*)(loc)))
#define DATA_LOCATION_TO_VALUE(loc)      (*((ValueType*)(loc)))
/* Used size is stored in the first location of the data array. */
#define LEAF_USED_SIZE                   (DATA_LOCATION_TO_UL(data) + sizeof(unsigned long))
#define OTHER_LEAF_USED_SIZE(leaf_ptr)   (DATA_LOCATION_TO_UL(leaf_ptr->data) + sizeof(unsigned long))
#define DATA_AFTER_LEAF_USED_SIZE        (data + sizeof(unsigned long))

template <typename ValueType>
unsigned char *TrieLeaf<ValueType>::find(const DatabaseKey &key, int firstCharacterIdx)
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

        currentLoc += sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc) + sizeof(ValueType);
    }

    return NULL;
}

template <typename ValueType>
void TrieLeaf<ValueType>::addBulk(unsigned char *source, unsigned long length)
{
    memcpy(data + LEAF_USED_SIZE, source, length);
    DATA_LOCATION_TO_UL(data) += length;
}

template <typename ValueType>
int TrieLeaf<ValueType>::compareKeys(unsigned char *currentCharacter,
        unsigned char *endCharacter, const DatabaseKey &key, int firstCharacterIdx)
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

template <typename ValueType>
ValueType TrieLeaf<ValueType>::get(const DatabaseKey &key, int firstCharacterIdx)
{
    unsigned char *searchResult = find(key, firstCharacterIdx);
    if (searchResult == NULL) {
        return 0;
    }

    return DATA_LOCATION_TO_VALUE(searchResult + sizeof(unsigned long) + DATA_LOCATION_TO_UL(searchResult));
}

template <typename ValueType>
void TrieLeaf<ValueType>::add(const DatabaseKey &key, int firstCharacterIdx, ValueType value)
{
    assert(canFit(key, firstCharacterIdx));
    unsigned long activeKeyLength = key.length - firstCharacterIdx;

    DATA_LOCATION_TO_UL(data + LEAF_USED_SIZE) = activeKeyLength;

    memcpy((void*)(data + LEAF_USED_SIZE + sizeof(unsigned long)),
           (void*)(key.data + firstCharacterIdx), activeKeyLength);

    DATA_LOCATION_TO_VALUE(data + LEAF_USED_SIZE + sizeof(unsigned long) + activeKeyLength) = value;
    DATA_LOCATION_TO_UL(data) += sizeof(unsigned long) + activeKeyLength + sizeof(ValueType);
}

template <typename ValueType>
void TrieLeaf<ValueType>::update(const DatabaseKey &key, int firstCharacterIdx, ValueType value)
{
    unsigned char *searchResult = find(key, firstCharacterIdx);
    if (searchResult == NULL) {
        return;
    }

    DATA_LOCATION_TO_VALUE(searchResult + sizeof(unsigned long) + DATA_LOCATION_TO_UL(searchResult)) = value;
}

template <typename ValueType>
void TrieLeaf<ValueType>::remove(const DatabaseKey &key, int firstCharacterIdx)
{
    unsigned char *searchResult = find(key, firstCharacterIdx);
    if (searchResult == NULL) {
        return;
    }

    unsigned long deletedSlotSize = sizeof(unsigned long) + DATA_LOCATION_TO_UL(searchResult) + sizeof(ValueType);
    unsigned char *currentLoc = searchResult + deletedSlotSize;

    while (currentLoc < (data + LEAF_USED_SIZE)) {
        unsigned long currentSlotSize = sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc) + sizeof(ValueType);
        memmove(currentLoc - deletedSlotSize, currentLoc, currentSlotSize);
        currentLoc += currentSlotSize;
    }

    DATA_LOCATION_TO_UL(data) -= deletedSlotSize;
}

template <typename ValueType>
bool TrieLeaf<ValueType>::isEmpty()
{
    /* Empty node only stores its size in first few bytes. */
    return LEAF_USED_SIZE == sizeof(unsigned long);
}

template <typename ValueType>
bool TrieLeaf<ValueType>::canFit(const DatabaseKey &key, int firstCharacterIdx)
{
    return (key.length - firstCharacterIdx + sizeof(unsigned int) + sizeof(ValueType)) <= (sizeof(data) - LEAF_USED_SIZE);
}

template <typename ValueType>
void TrieLeaf<ValueType>::moveAllBelowToAnotherLeaf(const DatabaseKey &key,
        int firstCharacterIdx, TrieLeaf<ValueType> &anotherLeaf)
{
    unsigned char *currentLoc = DATA_AFTER_LEAF_USED_SIZE;
    unsigned long shift = 0;

    while (currentLoc < (data + LEAF_USED_SIZE)) {
        unsigned long currentSlotSize = sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc) + sizeof(ValueType);

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

template <typename ValueType>
unsigned char TrieLeaf<ValueType>::findBestSplitPoint(unsigned char leftmostPoint, unsigned char rightmostPoint)
{
    unsigned char *currentLoc = DATA_AFTER_LEAF_USED_SIZE;
    unsigned long sizesPerFirstCharacter[256];
    memset(sizesPerFirstCharacter, 0, sizeof(sizesPerFirstCharacter));

    while (currentLoc < (data + LEAF_USED_SIZE)) {
        unsigned long currentSlotSize = sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc) + sizeof(ValueType);

        assert(DATA_LOCATION_TO_UL(currentLoc) != 0); /* We can't get first character of an empty string. */

        sizesPerFirstCharacter[*(currentLoc + sizeof(unsigned long))] += currentSlotSize;

        currentLoc += currentSlotSize;
    }

    unsigned long currentAccumulatedSizes = 0;
    unsigned long leafWithoutHeaderSize = LEAF_USED_SIZE - sizeof(unsigned long);

    int potentialSplitPoint;


    for (potentialSplitPoint = leftmostPoint; potentialSplitPoint <= rightmostPoint; potentialSplitPoint++) {
        /* We loop with int and then cast, to avoid uchar overflow causing infinite loop */
        if ((sizesPerFirstCharacter[potentialSplitPoint] + currentAccumulatedSizes) >= (leafWithoutHeaderSize / 2)) {
            if (currentAccumulatedSizes > 0) {
                //return (unsigned char) i;
                break;
            } else {
                /* First character with more than zero occurrences happens to overwhelm the leaf node.
                 */
                potentialSplitPoint++;
                break;
            }
        }

        currentAccumulatedSizes += sizesPerFirstCharacter[potentialSplitPoint];
    }

    if (potentialSplitPoint > rightmostPoint)
        return rightmostPoint;

    if (potentialSplitPoint == leftmostPoint)
        return leftmostPoint + 1;

    return potentialSplitPoint;
}

template<typename ValueType>
ValueType TrieLeaf<ValueType>::stripLeadingCharacter()
{
    unsigned char *currentLoc = DATA_AFTER_LEAF_USED_SIZE;
    unsigned long long result = 0;
    unsigned long shift = 0;

    while (currentLoc < (data + LEAF_USED_SIZE)) {
        unsigned long currentKeyLen = DATA_LOCATION_TO_UL(currentLoc);
        unsigned long currentSlotSize = sizeof(unsigned long) + currentKeyLen + sizeof(ValueType);

        if (DATA_LOCATION_TO_UL(currentLoc) == 1) {
            /* Key ends exactly in the leaf. We are expanding vertically, thus this value should
             * be moved upwards, to the newly created node.
             */
            result = DATA_LOCATION_TO_VALUE(currentLoc + sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc));
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

template<typename ValueType>
TrieLeafNavigator<ValueType> TrieLeaf<ValueType>::produceNaviagor()
{
    return TrieLeafNavigator<ValueType>(DATA_AFTER_LEAF_USED_SIZE, this);
}

template<typename ValueType>
TrieLeafNavigator<ValueType>::TrieLeafNavigator(unsigned char *currentLoc, TrieLeaf<ValueType> *context)
        : currentLoc(currentLoc), context(context) {
}

template<typename ValueType>
unsigned char* TrieLeafNavigator<ValueType>::getPointer() {
    return currentLoc + sizeof(unsigned long);
}

template<typename ValueType>
unsigned int TrieLeafNavigator<ValueType>::getLength() {
    return DATA_LOCATION_TO_UL(currentLoc);
}

template<typename ValueType>
ValueType TrieLeafNavigator<ValueType>::getValue() {
    return DATA_LOCATION_TO_VALUE(currentLoc + sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc));
}

template<typename ValueType>
void TrieLeafNavigator<ValueType>::next() {
    currentLoc += sizeof(unsigned long) + DATA_LOCATION_TO_UL(currentLoc) + sizeof(ValueType);
}

template<typename ValueType>
bool TrieLeafNavigator<ValueType>::isEnd() {
    return currentLoc >= (context->data + OTHER_LEAF_USED_SIZE(context));
}

template class TrieLeaf<unsigned long long>;
template class TrieLeafNavigator<unsigned long long>;

}

/* namespace Db */
