/*
 * TrieLeaf.h
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#ifndef TRIELEAF_H_
#define TRIELEAF_H_

namespace Db {

// We might prefer to use actual page size.
#define TYPICAL_PAGE_SIZE 4096

class TrieLeaf {
public:
    unsigned char data[2 * TYPICAL_PAGE_SIZE];
};

} /* namespace Db */
#endif /* TRIELEAF_H_ */
