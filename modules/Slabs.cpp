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
    /* Loads information about current slab allocation. */

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
                /* No empty places within this slab were found => this slab is full. */
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

    /* Class ID is an index of the slab classes array, not the logarithm itself. */
    return logarithm - SLAB_START_POWER;
}

size_t Slabs::computeObjectHeader(char dest[sizeof(uint32_t)], size_t sourceSize)
{
    /* At the beginning of each stored object, we have its size.
     * We deduce the smallest integer type required to store objects' sizes within a slab.
     */

    if (sourceSize <= UINT8_MAX)
    {
        *((uint8_t*) &dest) = (uint8_t) sourceSize;
        return sizeof(uint8_t);
    } else
    if (sourceSize <= UINT16_MAX)
    {
        *((uint16_t*) &dest) = (uint16_t) sourceSize;
        return sizeof(uint16_t);
    } else
    if (sourceSize <= UINT32_MAX)
    {
        *((uint32_t*) &dest) = (uint32_t) sourceSize;
        return sizeof(uint32_t);
    } else
    {
        assert(false);
    }
}

void Slabs::saveData(char *source, size_t size)
{
    char extraDataWithSize[sizeof(uint32_t)];
    size_t extraSizeForHeader = computeObjectHeader(extraDataWithSize, size);

    SlabsClass &slabsClass = slabClasses[getClassId(size + extraSizeForHeader)];

    if (slabsClass.slabsPartial.empty()) {
        unsigned long long newSlabId = slabs->getNewBinByID();
        unsigned long long newSlabInfoId = slabsInfo->getNewBinByID();

        assert(newSlabId == newSlabInfoId);

        Slab *newSlab = slabs->getBin(newSlabId);
        SlabInfo *newSlabInfo = slabsInfo->getBin(newSlabInfoId);
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
