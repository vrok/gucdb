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
#include <algorithm>
#include <iostream>
using namespace std;

namespace Db {

/*
 * All leaf data is stored within Leaf::data array. It is just a chain of bytes,
 * you should be able to load such a chain of bytes, cast it to TrieLeaf and use it.
 *
 * Here's how it is organized (^ - leaf bytes beginning, $ - leaf bytes end, ):
 *
 * <leaf> = ^ <values> <hash_map> <used_size> $
 * <values> = <value> <values> | <unused_space (might be 0 bytes) >
 * <value> = <value_length> <value_contents> <value_key>
 * <hash_map> = <hash_map_elems> <hash_map_elems_count>
 * <hash_map_elems> = <hash_map_elem> <hash_map_elems> | <null>
 * <hash_map_elem> = <hashed_value> <value_offset>
 *
 */


struct MapElem
{
    unsigned short hashedValue;
    unsigned short valueOffset;

    friend bool operator<(const MapElem &a, const MapElem &b)
    {
        return a.hashedValue < b.hashedValue;
    }
};


#define SOF_USED_SIZE sizeof(unsigned short)
#define SOF_MAP_COUNT sizeof(unsigned short)
#define SOF_VALUE_LEN sizeof(unsigned short)
#define SOF_MAP_ELEM sizeof(MapElem)

#define DATA_LOCATION_TO_MAP(loc)        (*(MapElem*)(loc))
#define DATA_LOCATION_TO_US(loc)         (*((unsigned short*)(loc)))
#define DATA_LOCATION_TO_UL(loc)         (*((unsigned long*)(loc)))
#define DATA_LOCATION_TO_ULL(loc)        (*((unsigned long long*)(loc)))
#define DATA_LOCATION_TO_VALUE(loc)      (*((ValueType*)(loc)))

#define DATA_END (data + sizeof(data))

#define LEAF_USED_SIZE \
    (DATA_LOCATION_TO_US(DATA_END - SOF_USED_SIZE) + SOF_USED_SIZE)

/* below doesnt work */
#define OTHER_LEAF_USED_SIZE(leaf_ptr) \
    (DATA_LOCATION_TO_US(leaf_ptr->data + sizeof(leaf_ptr->data) - SOF_USED_SIZE) + SOF_USED_SIZE)

#define DATA_AFTER_LEAF_USED_SIZE        (data + sizeof(unsigned long))

#define MAP_COUNT \
    DATA_LOCATION_TO_US(DATA_END - SOF_USED_SIZE - SOF_MAP_COUNT)

#define MAP_END \
    ((MapElem*)(DATA_END - SOF_USED_SIZE - SOF_MAP_COUNT))

#define MAP_BEGIN \
    ((MapElem*)(DATA_END - SOF_USED_SIZE - SOF_MAP_COUNT - (sizeof(MapElem) * MAP_COUNT)))

#define FREE_SPACE_OFFSET \
    (LEAF_USED_SIZE - (MAP_COUNT * SOF_MAP_ELEM) - SOF_MAP_COUNT)

#define FREE_SPACE_START \
    (data + FREE_SPACE_OFFSET)




static unsigned short hash(const DatabaseKey &key, int firstCharacterIdx)
{
    unsigned short current = 0;
    for (int i = firstCharacterIdx; i < key.length; i++) {
        current = (unsigned short) key.data[i];
    }
    return current;
}

template <typename ValueType>
void TrieLeaf<ValueType>::mapAdd(const DatabaseKey &key, int firstCharacterIdx, unsigned short valueOffset)
{
    unsigned short hashed = hash(key, firstCharacterIdx);

    MapElem *currentFit = MAP_BEGIN - 1;

    for (MapElem *elem = MAP_BEGIN; elem < MAP_END; elem++) {
        if (elem->hashedValue >= hashed) {
            break;
        }

        currentFit = elem;
        *(elem - 1) = *elem;
    }

    currentFit->hashedValue = hashed;
    currentFit->valueOffset = valueOffset;

    MAP_COUNT++;
}

template <typename ValueType>
unsigned short TrieLeaf<ValueType>::mapGet(bool &found, int iteration, unsigned short hashed)
{
    MapElem comparator;
    comparator.hashedValue = hashed;
    comparator.valueOffset = 0; /* This shouldn't be used, MapElem's operator< accesses only hashedValue. */

    MapElem *firstMatch = lower_bound(MAP_BEGIN, MAP_END, comparator);

    if (firstMatch == MAP_END) {
        found = false;
        return 0;
    }

    firstMatch += iteration;

    if (firstMatch >= MAP_END) {
        found = false;
        return 0;
    }

    if (firstMatch->hashedValue != comparator.hashedValue) {
        found = false;
        return 0;
    }

    found = true;
    return firstMatch->valueOffset;
}

template <typename ValueType>
MapElem *TrieLeaf<ValueType>::mapFindElem(const MapElem &elem)
{
    MapElem *occurence = lower_bound(MAP_BEGIN, MAP_END, elem);
    if (occurence == MAP_END) {
        return NULL;
    }

    while ((occurence < MAP_END) && (occurence->valueOffset != elem.valueOffset)) {
        occurence++;
    }

    if (occurence->valueOffset != elem.valueOffset) {
        return NULL;
    }

    return occurence;
}

template <typename ValueType>
void TrieLeaf<ValueType>::mapRemove(const DatabaseKey &key, int firstCharacterIdx, unsigned short valueOffset)
{
    unsigned short hashed = hash(key, firstCharacterIdx);

    MapElem comparator;
    comparator.hashedValue = hashed;
    comparator.valueOffset = valueOffset;

    MapElem *firstMatch = mapFindElem(comparator);

    assert(NULL != firstMatch); /* The item we're removing should exist. */

    /* Shift everything below, overwriting the item to be deleted. */
    for (MapElem *elem = firstMatch - 1; elem >= MAP_BEGIN; elem--) {
        *(elem + 1) = *elem;
    }

    MAP_COUNT--;
}

template <typename ValueType>
void TrieLeaf<ValueType>::mapUpdate(unsigned short hashed, unsigned short currentValueOffset, unsigned short newValueOffset)
{
    MapElem comparator;
    comparator.hashedValue = hashed;
    comparator.valueOffset = currentValueOffset;

    MapElem *firstMatch = mapFindElem(comparator);
    assert(NULL != firstMatch); /* The item we're updating should exist. */

    firstMatch->valueOffset = newValueOffset;
}

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
unsigned short TrieLeaf<ValueType>::mapFindKeyValue(bool &found, const DatabaseKey &key, int firstCharacterIdx)
{
    int iteration = 0;
    unsigned short hashed = hash(key, firstCharacterIdx);
    int compare = 0;
    unsigned short valueOffset = 0;

    found = false;

    do {
        valueOffset = mapGet(found, iteration++, hashed);

        if (!found)
            return 0;

        compare = compareKeys(data + valueOffset + SOF_VALUE_LEN,
                              data + valueOffset + SOF_VALUE_LEN + DATA_LOCATION_TO_US(data + valueOffset),
                              key, firstCharacterIdx);
    } while (compare != 0);

    return valueOffset;
    //return & DATA_LOCATION_TO_VALUE(data + valueOffset + SOF_VALUE_LEN + DATA_LOCATION_TO_US(data + valueOffset));
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
    bool found = false;
    unsigned short valueOffset = mapFindKeyValue(found, key, firstCharacterIdx);

    if (!found) {
        return 0;
    }

    return DATA_LOCATION_TO_VALUE(data + valueOffset + SOF_VALUE_LEN + DATA_LOCATION_TO_US(data + valueOffset));
}

template <typename ValueType>
void TrieLeaf<ValueType>::add(const DatabaseKey &key, int firstCharacterIdx, ValueType value)
{
    assert(canFit(key, firstCharacterIdx));
    unsigned short activeKeyLength = key.length - firstCharacterIdx;

    DATA_LOCATION_TO_US(FREE_SPACE_START) = activeKeyLength;

    memcpy((void*)(FREE_SPACE_START + SOF_VALUE_LEN),
           (void*)(key.data + firstCharacterIdx), activeKeyLength);

    DATA_LOCATION_TO_VALUE(FREE_SPACE_START + SOF_VALUE_LEN + activeKeyLength) = value;

    mapAdd(key, firstCharacterIdx, FREE_SPACE_OFFSET);

    DATA_LOCATION_TO_US(DATA_END - SOF_USED_SIZE) += SOF_VALUE_LEN + activeKeyLength + sizeof(ValueType) + SOF_MAP_ELEM;
}

template <typename ValueType>
void TrieLeaf<ValueType>::update(const DatabaseKey &key, int firstCharacterIdx, ValueType value)
{
    bool found = false;
    unsigned short valueOffset = mapFindKeyValue(found, key, firstCharacterIdx);

    if (found) {
        DATA_LOCATION_TO_VALUE(data + valueOffset + SOF_VALUE_LEN + DATA_LOCATION_TO_US(data + valueOffset)) = value;
    }
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
    return (key.length - firstCharacterIdx + SOF_VALUE_LEN + sizeof(ValueType) + SOF_MAP_ELEM)
            <= (sizeof(data) - LEAF_USED_SIZE);
    //return (key.length - firstCharacterIdx + sizeof(unsigned int) + sizeof(ValueType)) <= (sizeof(data) - LEAF_USED_SIZE);
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
