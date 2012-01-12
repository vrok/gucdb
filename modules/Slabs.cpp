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

Slabs::Slabs(BinFile<Slab> slabs, BinFile<SlabInfo> slabsInfo)
        : slabs(slabs), slabsInfo(slabsInfo)
{

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
