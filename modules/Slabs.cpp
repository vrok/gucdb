/*
 * Slabs.cpp
 *
 *  Created on: 10-01-2012
 *      Author: m
 */

#include "Slabs.h"

#include <iostream>
using namespace std;

#include <cmath>
#include <cstring>

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
                slabClasses[getSuitableClass(slabInfo->slabObjectSize)].slabsPartial.push_back(newSlab);
            } else {
                slabClasses[getSuitableClass(slabInfo->slabObjectSize)].slabsFull.push_back(newSlab);
            }
        }
    }
}

int Slabs::getSuitableClass(size_t objectSize)
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

size_t Slabs::getSizeOfClass(int classId)
{
	return pow(2ul, classId + SLAB_START_POWER);
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

unsigned long long Slabs::createNewSlab(int classId)
{
	SlabsClass &slabsClass = slabClasses[classId];

	unsigned long long newSlabId = slabs->getNewBinByID();
	unsigned long long newSlabInfoId = slabsInfo->getNewBinByID();

	assert(newSlabId == newSlabInfoId);

	Slab *newSlab = slabs->getBin(newSlabId);
	SlabInfo *newSlabInfo = slabsInfo->getBin(newSlabInfoId);

	newSlabInfo->slabObjectSize = getSizeOfClass(classId);

	unsigned long maxNumOfObjectsInSlab = SLAB_SIZE / getSizeOfClass(classId);

	SlabIdAndFreeObjectsList *slabPair = new SlabIdAndFreeObjectsList;
	slabPair->first = newSlabId;
	slabPair->second.reserve(maxNumOfObjectsInSlab);

	for (unsigned long slabInnerID = 0; slabInnerID < maxNumOfObjectsInSlab; slabInnerID++) {
	    slabPair->second.push_back(slabInnerID);
	}

	slabsClass.slabsPartial.push_back(slabPair);

	cout << "Created new slab " << newSlabId << endl;

	return newSlabId;
}

char* Slabs::getLocationInSlabByInnerID(Slab &slab, SlabInfo &slabInfo, unsigned long slabInnerID)
{
    return slab.data + (slabInfo.slabObjectSize * slabInnerID);
}

ObjectID Slabs::saveData(char *source, size_t size)
{
    char extraDataContainingObjectSize[sizeof(uint32_t)];
    size_t sizeOfExtraData = computeObjectHeader(extraDataContainingObjectSize, size);
	size_t spaceRequiredForNewData = size + sizeOfExtraData;

	int classId = getSuitableClass(spaceRequiredForNewData);

    SlabsClass &slabClass = slabClasses[classId];

    if (slabClass.slabsPartial.empty()) {
        createNewSlab(classId);
    }

    SlabIdAndFreeObjectsList *slabPair = slabClass.slabsPartial.back();
    unsigned long long slabID = slabPair->first;
    unsigned long innerSlabID = slabPair->second.back();
    slabPair->second.pop_back();

    if (slabPair->second.empty()) {
        /* Slab is full, move it from vector with partially filled slabs. */
        slabClass.slabsPartial.pop_back();
        slabClass.slabsFull.push_back(slabPair);
    }

    Slab *targetSlab = slabs->getBin(slabID);
    SlabInfo *targetSlabInfo = slabsInfo->getBin(slabID);

    cout << "Selected slab  " << slabID << ", innerSlabID: " << innerSlabID << "   slabClass: " << classId << endl;

    char *targetLocation = getLocationInSlabByInnerID(*targetSlab, *targetSlabInfo, innerSlabID);

    memcpy(targetLocation, extraDataContainingObjectSize, sizeOfExtraData);
    memcpy(targetLocation + sizeOfExtraData, source, size);

    return ObjectID(slabID, innerSlabID);
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



using namespace Db;

int main()
{
#if 0
    mainIndex = new Trie<ValueAddress>(new BinFile<TrieNode<ValueAddress> >(dbDirectory + "/main.nodes",
                                               new BinFileMap(dbDirectory + "/main.nodes.map"),
                                               BinFile<TrieNode<ValueAddress> >::minimalIndexExpandSize() * 32),
                                       new BinFile<TrieLeaf<ValueAddress> >(dbDirectory + "/main.leaves",
                                               new BinFileMap(dbDirectory + "/main.leaves.map"),
                                               BinFile<TrieLeaf<ValueAddress> >::minimalIndexExpandSize() * 1));
#endif

    Slabs *slabs = new Slabs(new BinFile<Slab>("/tmp/main.slabs", new BinFileMap("/tmp/main.slabs.map"), BinFile<Slab>::minimalIndexExpandSize()),
                             new BinFile<SlabInfo>("/tmp/main.slabinfos", new BinFileMap("/tmp/main.slabinfos.map"), BinFile<Slab>::minimalIndexExpandSize()));

    slabs->slabs->openMMapedFile();
    slabs->slabsInfo->openMMapedFile();

    char *test = "trelemorele raz dwa trzy cztery piec szesc siedem";

    for (int i = 0; i < 60000; i++) {
        slabs->saveData(test, strlen(test) - 1);
        cout << "aa" << endl;
    }



    return 0;
}

