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

#include "SystemParams.h"
#include "BinFile.h"
#include "BinFileAllocators.h"

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


struct ObjectID
{
    ObjectID(unsigned long long slabID, unsigned long slabInnerID)
        : slabID(slabID), slabInnerID(slabInnerID) {}

    ObjectID() {}

    ObjectID(uint64_t intRepr)
        : slabID(intRepr >> 20), slabInnerID((intRepr << 44) >> 44) {}

    operator uint64_t()
    {
        /* 20 is roughly sizeof(slabInnerID) (using sizeof operator doesn't work for bitfields) */
        return (slabID << 20) | slabInnerID;
    }

    /* We'd like to sizeof(ObjectID) == 64 */
    unsigned long long slabID : 44;
    unsigned long slabInnerID : 20;
};

typedef vector<ObjectID> SlabsClass;

class Slabs
{
private:
    static size_t computeObjectHeader(char dest[sizeof(uint32_t)], size_t sourceSize);
    size_t readObjectSizeAndPointToData(char *&source, size_t classSize);

	unsigned long long createNewSlab(int classId);
	char *getLocationInSlabByInnerID(Slab &slab, SlabInfo &slabInfo, unsigned long slabInnerID);

	Slabs(const Slabs&);             /* Non-copiable */
	Slabs & operator=(const Slabs&); /* Non-copiable */

public:
    BinFile<Slab> *slabs;
    BinFile<SlabInfo> *slabsInfo;
    SlabsClass slabClasses[SLAB_END_POWER - SLAB_START_POWER + 1];

    void initialize();

    static int getSuitableClass(size_t objectSize);
	static size_t getSizeOfClass(int classId);

    Slabs(BinFile<Slab> *slabs, BinFile<SlabInfo> *slabsInfo);

    ObjectID saveData(const char *source, size_t size);
    size_t readData(char *&source, ObjectID objectID);
    void removeData(ObjectID objectID);

    void dumpAllInfo(ostream &where);
    void dumpSlabInfo(ostream &where, unsigned long long slabID);
};

struct Slab
{
    typedef LinearAllocator<Slab> AllocatorType;

    char data[SLAB_SIZE];
};

struct SlabInfo
{
    typedef LinearAllocator<SlabInfo> AllocatorType;

    size_t slabObjectSize;
    size_t allocated;

    /* Theoretically, each Slab Class has distinct needs for its free objects map.
     * But we want to store these maps in a BinFile, thus we need to put them in
     * constant-sized objects. Size of such constant-sized objects has to be large
     * enough to store information about the Slab Class which requires most space
     * (it is the smallest supported Slab Class).
     */
    unsigned char slabObjectsMap[SLAB_SIZE / SLAB_START_OBJECT_SIZE];

    /* Every type stored using BinFile has to be properly padded. See asserts
     * in BinFile constructor for details.
     */
    char someDataJustToMakeStructNicelyPadded[TYPICAL_PAGE_SIZE - ((2 * sizeof(size_t)
            + (SLAB_SIZE / SLAB_START_OBJECT_SIZE)) % TYPICAL_PAGE_SIZE)];
};

} /* namespace Db */
#endif /* SLABS_H_ */
