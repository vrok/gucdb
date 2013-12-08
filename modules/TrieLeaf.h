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
 * TrieLeaf.h
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#ifndef TRIELEAF_H_
#define TRIELEAF_H_

#include "SystemParams.h"
#include "DatabaseKey.h"

namespace Db {

template <typename ValueType>
class TrieLeafNavigator;

template <typename ValueType>
class Trie;

struct MapElem;

/* Important: ValueType should be small, as it is often passed by value (or copied in some other way).
 * Ideally, it should be derived from a built-in type, like an offset or a pointer.
 */
template <typename ValueType>
class TrieLeaf {
private:
	friend class TrieLeafTest; /* Unit test */

    friend class Trie<ValueType>;
    friend class TrieLeafNavigator<ValueType>;

    TrieLeaf(const TrieLeaf &);             /* Non-copiable */
    TrieLeaf & operator=(const TrieLeaf &); /* Non-copiable */

    unsigned char *find(const DatabaseKey &key, int firstCharacterIdx);
    void addBulk(unsigned char *source, unsigned long length);
    int compareKeys(unsigned char *currentCharacter, unsigned char *endCharacter, const DatabaseKey &key, int firstCharacterIdx);

    void mapAdd(unsigned short hashed, unsigned short valueOffset);
    unsigned short mapGet(bool &found, int iteration, unsigned short hashed);
    MapElem *mapFindElem(const MapElem &elem);
    void mapRemove(unsigned short hashed, unsigned short valueOffset);
    void mapUpdate(unsigned short hashed, unsigned short currentValueOffset, unsigned short newValueOffset);
    unsigned char *getFreeMemStart();

    unsigned short mapFindKeyValue(bool &found, const DatabaseKey &key, int firstCharacterIdx);

public:
    typedef ExponentialAllocator<TrieLeaf<ValueType> > AllocatorType;

    unsigned char data[2 * TYPICAL_PAGE_SIZE];
    //unsigned char data[90];

    TrieLeaf() {}

    bool isEmpty();
    bool canFit(const DatabaseKey &key, int firstCharacterIdx);
    ValueType get(const DatabaseKey &key, int firstCharactrdIdx);
    void add(const DatabaseKey &key, int firstCharacterIdx, ValueType value);
    void update(const DatabaseKey &key, int firstCharacterIdx, ValueType value);
    void remove(const DatabaseKey &key, int firstCharacterIdx);
    void moveAllBelowToAnotherLeaf(const DatabaseKey &key, int firstCharacterIdx, TrieLeaf<ValueType> &anotherLeaf);
    unsigned char findBestSplitPoint(unsigned char leftmostPoint, unsigned char rightmostPoint);
    ValueType stripLeadingCharacter();

    TrieLeafNavigator<ValueType> produceNaviagor();
};

template <typename ValueType>
struct TrieLeafNavigator {
private:
    friend class TrieLeaf<ValueType>;
    unsigned char *currentLoc;
    TrieLeaf<ValueType> *context;

    TrieLeafNavigator(unsigned char *currentLoc, TrieLeaf<ValueType> *context);
public:

    unsigned char *getPointer();
    unsigned short getLength();
    ValueType getValue();

    void next();
    bool isEnd();
};


} /* namespace Db */
#endif /* TRIELEAF_H_ */
