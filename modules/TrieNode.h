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

struct TrieNode {
    unsigned long long int children[NODE_SIZE];
};

} /* namespace Db */
#endif /* TRIENODE_H_ */
