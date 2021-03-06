/*
    Copyright 2013 Marcin Wrochniak

    This file is part of Guc DB.

    Guc DB is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    Guc DB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * Slabs.cpp
 *
 *  Created on: 10-01-2012
 *      Author: m
 */

#include "Slabs.h"

#include <iostream>
#include <string>
#include <sstream>
using namespace std;

#include <cmath>
#include <cstring>

namespace Db {

#define SLAB_START_POWER 2
#define SLAB_END_POWER 20

Slabs::Slabs(BinFile<Slab> *slabs, BinFile<SlabInfo> *slabsInfo)
        : slabs(slabs), slabsInfo(slabsInfo)
{
    slabs->openMMapedFile();
    slabsInfo->openMMapedFile();

    initialize();
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
            unsigned long maxNumOfObjectsInSlab = SLAB_SIZE / slabInfo->slabObjectSize;

            for (unsigned long currentByte = 0; currentByte <= maxNumOfObjectsInSlab / 8; currentByte++) {
                for (int currentBit = 0; currentBit < 8; currentBit++) {
                    if (0 == ((1 << currentBit) & slabInfo->slabObjectsMap[currentByte])) {
                        /* To understand why this address computation makes sense, please see the comments
                         * in Slabs::getLocationInSlabByInnerID().
                         */
                        slabClasses[getSuitableClass(slabInfo->slabObjectSize)]
                                    .push_back(ObjectID(slabId, (currentByte * 8) + currentBit + 1));
                    }
                }
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
        *((uint8_t*) dest) = (uint8_t) sourceSize;
        return sizeof(uint8_t);
    } else
    if (sourceSize <= UINT16_MAX)
    {
        *((uint16_t*) dest) = (uint16_t) sourceSize;
        return sizeof(uint16_t);
    } else
    if (sourceSize <= UINT32_MAX)
    {
        *((uint32_t*) dest) = (uint32_t) sourceSize;
        return sizeof(uint32_t);
    } else
    {
        assert(false);
    }
}

size_t Slabs::readObjectSizeAndPointToData(char* &source, size_t classSize)
{
    size_t objectSize = 0;

    if ((classSize - sizeof(uint8_t)) <= UINT8_MAX)
    {
        objectSize = (size_t) *((uint8_t*) source);
        source += sizeof(uint8_t);
    } else
    if ((classSize - sizeof(uint16_t)) <= UINT16_MAX)
    {
        objectSize = (size_t) *((uint16_t*) source);
        source += sizeof(uint16_t);
    } else
    if ((classSize - sizeof(uint32_t)) <= UINT32_MAX)
    {
        objectSize = (size_t) *((uint32_t*) source);
        source += sizeof(uint32_t);
    } else
    {
        assert(false);
    }

    return objectSize;
}

unsigned long long Slabs::createNewSlab(int classId)
{
	SlabsClass &slabsClass = slabClasses[classId];

	unsigned long long newSlabId = slabs->getNewBinByID();
	unsigned long long newSlabInfoId = slabsInfo->getNewBinByID();

	assert(newSlabId == newSlabInfoId);

	//Slab *newSlab = slabs->getBin(newSlabId);
	SlabInfo *newSlabInfo = slabsInfo->getBin(newSlabInfoId);

	newSlabInfo->slabObjectSize = getSizeOfClass(classId);

	unsigned long maxNumOfObjectsInSlab = SLAB_SIZE / getSizeOfClass(classId);

	for (unsigned long slabInnerID = 1; slabInnerID <= maxNumOfObjectsInSlab; slabInnerID++) {
	    slabsClass.push_back(ObjectID(newSlabId, slabInnerID));
	}

	return newSlabId;
}

char* Slabs::getLocationInSlabByInnerID(Slab &slab, SlabInfo &slabInfo, unsigned long slabInnerID)
{
    /* slabInnerID is always larger by 1 than the actual offset (hence its decremented before its
     * used below). The reason is that without this an ObjectID with both slabID and slabInnerID equal
     * to 0 would be valid. We want to preserve it - it is needed in the Trie module, it represents
     * the null ObjectID.
     * (If you're updating this comment, please also check whether comments in different places
     * in this file that refer to this one need updating.)
     */
    return slab.data + (slabInfo.slabObjectSize * (slabInnerID - 1));
}

ObjectID Slabs::saveData(const char *source, size_t size)
{
    char extraDataContainingObjectSize[sizeof(uint32_t)];
    size_t sizeOfExtraData = computeObjectHeader(extraDataContainingObjectSize, size);
	size_t spaceRequiredForNewData = size + sizeOfExtraData;
	int classID = getSuitableClass(spaceRequiredForNewData);

    SlabsClass &slabClass = slabClasses[classID];

    if (slabClass.empty()) {
        createNewSlab(classID);
    }

    ObjectID objectID = slabClass.back();
    slabClass.pop_back();
    unsigned long long slabID = objectID.slabID;
    unsigned long innerSlabID = objectID.slabInnerID;
    /* See the comments in getLocationInSlabByInnerID to understand this. */
    unsigned long innerSlabOffset = innerSlabID - 1;

    Slab *targetSlab = slabs->getBin(slabID);
    SlabInfo *targetSlabInfo = slabsInfo->getBin(slabID);

    char *targetLocation = getLocationInSlabByInnerID(*targetSlab, *targetSlabInfo, innerSlabID);

    memcpy(targetLocation, extraDataContainingObjectSize, sizeOfExtraData);
    memcpy(targetLocation + sizeOfExtraData, source, size);

    targetSlabInfo->allocated += targetSlabInfo->slabObjectSize;
    targetSlabInfo->slabObjectsMap[innerSlabOffset / 8] |= 1 << (innerSlabOffset % 8);

    return ObjectID(slabID, innerSlabID);
}

size_t Slabs::readData(char *&source, ObjectID objectID)
{
    assert(slabs->isBinIDSafeAndAllocated(objectID.slabID));

    Slab *sourceSlab = slabs->getBin(objectID.slabID);
    SlabInfo *sourceSlabInfo = slabsInfo->getBin(objectID.slabID);

    char *rawData = getLocationInSlabByInnerID(*sourceSlab, *sourceSlabInfo, objectID.slabInnerID);

    size_t dataSize = readObjectSizeAndPointToData(rawData, sourceSlabInfo->slabObjectSize);

    source = rawData;
    return dataSize;
}

void Slabs::removeData(ObjectID objectID)
{
    assert(slabs->isBinIDSafeAndAllocated(objectID.slabID));

    unsigned long long slabID = objectID.slabID;
    unsigned long innerSlabOffset = objectID.slabInnerID - 1;

    SlabInfo *slabInfo = slabsInfo->getBin(slabID);
    Slab *slab = slabs->getBin(slabID);

    slabInfo->allocated -= slabInfo->slabObjectSize;
    slabInfo->slabObjectsMap[innerSlabOffset / 8] &= ~(1 << (innerSlabOffset % 8));

    char *rawData = getLocationInSlabByInnerID(*slab, *slabInfo, objectID.slabInnerID);
    /* Erase the deallocated space. Logic of the application should prevent clients
     * re-using this ObjectID from being able to access the old data, but if something
     * should be removed, let's make it removed.
     */
    memset(rawData, 0, slabInfo->slabObjectSize);

    slabClasses[getSuitableClass(slabInfo->slabObjectSize)].push_back(objectID);
}

void Slabs::dumpAllInfo(ostream &where)
{
    /* Loads information about current slab allocation. */

    const BinFileMap &slabInfoMap = slabsInfo->getBinFileMap();

    for (BinFileMap::Iterator it = slabInfoMap.getIterator(); it != slabInfoMap.end(); ++it) {
        /* Check if it's a non-empty bin (slab). */
        if ((*it).second) {
            unsigned long long slabId = (*it).first;

            SlabInfo *slabInfo = slabsInfo->getBin(slabId);

            if (! slabs->isBinIDSafeAndAllocated(slabId)) {
                where << "Slab ID " << slabId << " is invalid or not used.";
                continue;
            }

            //SlabInfo *slabInfo = slabsInfo->getBin(slabID);
            where << "Size of this slab's class is " << slabInfo->slabObjectSize << endl;
            where << "It allocated " << slabInfo->allocated << endl;
#if 0
            for (unsigned long currentByte = 0; currentByte <= slabInfo->slabObjectSize / 8; currentByte++) {
                for (int currentBit = 0; currentBit < 8; currentBit++) {
                    if (0 == ((1 << currentBit) & slabInfo->slabObjectsMap[currentByte])) {
                        slabClasses[getSuitableClass(slabInfo->slabObjectSize)]
                                    .push_back(ObjectID(slabId, (currentByte * 8) + currentBit));
                    }
                }
            }
#endif
        }
    }
}

void Slabs::dumpSlabInfo(ostream &where, unsigned long long slabID)
{
    if (! slabs->isBinIDSafeAndAllocated(slabID)) {
        where << "Slab ID " << slabID << " is invalid or not used.";
        return;
    }

    SlabInfo *slabInfo = slabsInfo->getBin(slabID);
    where << "Size of this slab's class is " << slabInfo->slabObjectSize << endl;
}

} /* namespace Db */


//#define TEST_SLABS
#ifdef TEST_SLABS

using namespace Db;

int main()
{
    Slabs *slabs = new Slabs(new BinFile<Slab>("/tmp/main.slabs",
                                               new BinFileMap("/tmp/main.slabs.map"),
                                               BinFile<Slab>::minimalIndexExpandSize()),
                             new BinFile<SlabInfo>("/tmp/main.slabinfos",
                                                   new BinFileMap("/tmp/main.slabinfos.map"),
                                                   BinFile<SlabInfo>::minimalIndexExpandSize()));

    long command_counter = 0;

    while (true) {
        //cerr << "> ";

        string command;
        cin >> command;

        if (command == "write") {
            string value;
            cin >> value;

            assert(0 != value.length());

            ObjectID oid = slabs->saveData(value.c_str(), value.length());

            cout << (uint64_t) oid << endl;
        } else
        if (command == "read") {
            uint64_t oidAsUInt;

            cin >> oidAsUInt;

            ObjectID oid(oidAsUInt);
            char *result = 0;
            size_t resultSize = slabs->readData(result, oid);

            if (0 == resultSize) {
                cerr << "slab_id: " << oid.slabID << " slab_inner_id: " << oid.slabInnerID << endl;
                slabs->dumpSlabInfo(cerr, oid.slabID);
                assert(0 != resultSize);
            }

            for (int i = 0; i < resultSize; i++) {
                cout << result[i];
            }

            cout << endl;
        } else
        if (command == "remove") {
            uint64_t oidAsUInt;
            cin >> oidAsUInt;

            ObjectID oid(oidAsUInt);

            slabs->removeData(oid);
            cout << endl;
        } else
        if (command == "exit") {
            return 0;
        } else {
            cerr << "Unknown command: '" << command << "'" << endl;
        }
    }
    return 0;
}

#endif
