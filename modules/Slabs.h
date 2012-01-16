/*
 * Slabs.h
 *
 *  Created on: 10-01-2012
 *      Author: m
 */

#ifndef SLABS_H_
#define SLABS_H_

#define __STDC_LIMIT_MACROS
#include <stdint.h>

//#include <cstdint>
#include <cassert>
#include <utility>
#include <vector>
using namespace std;

#include "BinFile.h"

namespace Db {

#define SLAB_SIZE (1024 * 1024)

#define SLAB_START_POWER 2
/* SLAB_START_OBJECT_SIZE is equal to (2 ^ SLAB_START_POWER) */
#define SLAB_START_OBJECT_SIZE 4

#define SLAB_END_POWER 20
/* SLAB_END_OBJECT_SIZE is equal to (2 ^ SLAB_END_POWER) */
#define SLAB_END_OBJECT_SIZE (1024 * 1024)

struct Slab;
struct SlabInfo;

typedef pair<unsigned long long, vector<unsigned long> > SlabIdAndFreeObjectsList;

struct SlabsClass
{
    vector<SlabIdAndFreeObjectsList*> slabsFull;
    vector<SlabIdAndFreeObjectsList*> slabsPartial;
};

class Slabs
{
private:
    void initialize();
    static size_t computeObjectHeader(char dest[sizeof(uint32_t)], size_t sourceSize);

public:
    BinFile<Slab> *slabs;
    BinFile<SlabInfo> *slabsInfo;
    SlabsClass slabClasses[SLAB_END_POWER - SLAB_START_POWER + 1];

    static int getClassId(size_t objectSize);

    Slabs(BinFile<Slab> *slabs, BinFile<SlabInfo> *slabsInfo);

    void saveData(char *source, size_t size);
    void readData();
};

struct Slab
{
    char data[SLAB_SIZE];
};

struct SlabInfo
{
    size_t slabObjectSize;

    /* Theoretically, each Slab Class has distinct needs for its free objects map.
     * But we want to store these maps in a BinFile, thus we need to put them in
     * constant-sized objects. Size of such constant-sized objects has to be large
     * enough to store information about the Slab Class which requires most space
     * (it is the smallest supported Slab Class).
     */
    unsigned char slabObjectsMap[SLAB_SIZE / SLAB_START_OBJECT_SIZE];
};

} /* namespace Db */
#endif /* SLABS_H_ */
