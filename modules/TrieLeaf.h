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

/* Important: ValueType should be small, as it is often passed by value (or copied in some other way).
 * Ideally, it should be derived from a built-in type, like an offset or a pointer.
 */
template <typename ValueType>
class TrieLeaf {
private:
    friend class Trie<ValueType>;

    unsigned char *find(const DatabaseKey &key, int firstCharacterIdx);
    void addBulk(unsigned char *source, unsigned long length);
    int compareKeys(unsigned char *currentCharacter, unsigned char *endCharacter, const DatabaseKey &key, int firstCharacterIdx);

public:
    unsigned char data[2 * TYPICAL_PAGE_SIZE];
    //unsigned char data[90];

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
    unsigned int getLength();
    ValueType getValue();

    void next();
    bool isEnd();
};


} /* namespace Db */
#endif /* TRIELEAF_H_ */
