/*
 * TrieLeaf.h
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#ifndef TRIELEAF_H_
#define TRIELEAF_H_

#include "DatabaseKey.h"

namespace Db {

// We might prefer to use actual page size.
#define TYPICAL_PAGE_SIZE 4096

class TrieLeafNavigator;


class TrieLeaf {
private:
    unsigned char *find(const DatabaseKey &key, int firstCharacterIdx);
    void addBulk(unsigned char *source, unsigned long length);
    int compareKeys(unsigned char *currentCharacter, unsigned char *endCharacter, const DatabaseKey &key, int firstCharacterIdx);

public:
    //unsigned char data[2 * TYPICAL_PAGE_SIZE];
    unsigned char data[30];

    bool isEmpty();
    bool canFit(const DatabaseKey &key, int firstCharacterIdx);
    unsigned long long get(const DatabaseKey &key, int firstCharactrdIdx);
    void add(const DatabaseKey &key, int firstCharacterIdx, unsigned long long value);
    void remove(const DatabaseKey &key, int firstCharacterIdx);
    void moveAllBelowToAnotherLeaf(const DatabaseKey &key, int firstCharacterIdx, TrieLeaf &anotherLeaf);
    unsigned char findBestSplitPoint(unsigned char leftmostPoint, unsigned char rightmostPoint);
    unsigned long long stripLeadingCharacter();

    TrieLeafNavigator produceNaviagor();
};


struct TrieLeafNavigator {
private:
    friend class TrieLeaf;
    unsigned char *currentLoc;
    TrieLeaf *context;

    TrieLeafNavigator(unsigned char *currentLoc, TrieLeaf *context);
public:

    unsigned char *getPointer();
    unsigned int getLength();
    unsigned long long getValue();

    void next();
    bool isEnd();
};


} /* namespace Db */
#endif /* TRIELEAF_H_ */
