/*
 * TriePointer.h
 *
 *  Created on: 10-12-2011
 *      Author: m
 */

#ifndef TRIEPOINTER_H_
#define TRIEPOINTER_H_

namespace Db {

struct TriePointer {
    TriePointer(bool pointee_is_leaf, unsigned long long int pointee_id);

    unsigned char leaf : 1;
    unsigned long long int link : 63;

    bool operator== (const TriePointer &pointer);
    bool operator!= (const TriePointer &pointer);
};

} /* namespace Db */
#endif /* TRIEPOINTER_H_ */
