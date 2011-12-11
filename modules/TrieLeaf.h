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

class TrieLeaf {
private:
    void vacuum();

    unsigned char *find(const DatabaseKey &key, int firstCharactedIdx);

public:
    unsigned char data[2 * TYPICAL_PAGE_SIZE];

    bool canFit(const DatabaseKey &key, int firstCharacterIdx);
    unsigned long long get(const DatabaseKey &key, int firstCharactedIdx);
    void add(const DatabaseKey &key, int firstCharacterIdx, unsigned long long value);
    void remove(const DatabaseKey &key, int firstCharacterIdx);
    void moveAllBelowToAnotherLeaf(const DatabaseKey &key, int firstCharacterIdx, TrieLeaf &anotherLeaf);
};

} /* namespace Db */
#endif /* TRIELEAF_H_ */
