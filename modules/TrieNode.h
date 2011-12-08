/*
 * TrieNode.h
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#ifndef TRIENODE_H_
#define TRIENODE_H_

namespace Db {

#define NODE_SIZE 0xff

struct TriePointer {
    unsigned char leaf : 1;
    unsigned long long int link : 63;
};

struct TrieNode {
    TriePointer children[NODE_SIZE];
};

} /* namespace Db */
#endif /* TRIENODE_H_ */
