/*
 * Slabs.cpp
 *
 *  Created on: 10-01-2012
 *      Author: m
 */

#include "Slabs.h"

namespace Db {

#define SLAB_START_POWER 2
#define SLAB_END_POWER 20

Slabs::Slabs(BinFile<Slab> *slabs, BinFile<SlabInfo> *slabsInfo)
        : slabs(slabs), slabsInfo(slabsInfo)
{

}

void Slabs::initialize()
{
    const BinFileMap &slabInfoMap = slabsInfo->getBinFileMap();
    for (BinFileMap::Iterator it = slabInfoMap.getIterator(); it != slabInfoMap.end(); ++it) {
        /* Check if it's a non-empty bin (slab). */
        if ((*it).second) {
            unsigned long long slabId = (*it).first;

            SlabInfo *slabInfo = slabsInfo->getBin(slabId);

            SlabIdAndFreeObjectsList *newSlab = new SlabIdAndFreeObjectsList;
            newSlab->first = slabId;
            newSlab->second.reserve(SLAB_SIZE / slabInfo->slabObjectSize);

            for (unsigned long currentByte = 0; currentByte <= slabInfo->slabObjectSize / 8; currentByte++) {
                for (int currentBit = 1; currentBit <= 8; currentBit++) {
                    if (0 == ((1 << currentBit) & slabInfo->slabObjectsMap[currentByte])) {
                        newSlab->second.push_back((currentByte * 8) + currentBit);
                    }
                }
            }

            if (newSlab->second.empty()) {
                slabClasses[getClassId(slabInfo->slabObjectSize)].slabsPartial.push_back(newSlab);
            } else {
                slabClasses[getClassId(slabInfo->slabObjectSize)].slabsFull.push_back(newSlab);
            }
        }
    }
}

int Slabs::getClassId(size_t objectSize)
{
    size_t size = SLAB_START_OBJECT_SIZE;
    int logarithm = SLAB_START_POWER;

    while (objectSize > size) {
        logarithm++;
        size *= 2;
    }

    /* Class ID is the index within an array, not the logarithm itself. */
    return logarithm - SLAB_START_POWER;
}

void Slabs::saveData(char *source, size_t size)
{
    size_t extraSpaceForSize = 0;
    char extraDataWithSize[sizeof(uint32_t)];

    if (size <= UINT8_MAX)
    {
        extraSpaceForSize = sizeof(uint8_t);
        *((uint8_t*) &extraDataWithSize) = (uint8_t) size;
    } else
    if (size <= UINT16_MAX)
    {
        extraSpaceForSize = sizeof(uint16_t);
        *((uint16_t*) &extraDataWithSize) = (uint16_t) size;
    } else
    if (size <= UINT32_MAX)
    {
        extraSpaceForSize = sizeof(uint32_t);
        *((uint32_t*) &extraDataWithSize) = (uint32_t) size;
    } else
    {
        assert(false);
    }
}

void Slabs::readData()
{
    size_t slabObjectSize = 0;
    if ((slabObjectSize - sizeof(uint8_t)) <= UINT8_MAX)
    {

    } else
    if ((slabObjectSize - sizeof(uint16_t)) <= UINT16_MAX)
    {

    } else
    if ((slabObjectSize - sizeof(uint32_t)) <= UINT32_MAX)
    {

    } else
    {
        assert(false);
    }
}

} /* namespace Db */
