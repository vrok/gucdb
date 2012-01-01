/*
 * TrieNode.h
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#ifndef TRIENODE_H_
#define TRIENODE_H_

#include "TriePointer.h"

namespace Db {

#define NODE_SIZE 0xff + 1

struct TrieNode {
    TriePointer children[NODE_SIZE];

    unsigned long long values[NODE_SIZE];

    void setChild(unsigned char character, const TriePointer &childPointer);

    void setChildrenRange(unsigned char firstCharacter, unsigned char lastCharacter, const TriePointer &childPointer);

    bool isLinkPure(unsigned char character);

    unsigned char checkLeftmostCharWithLink(unsigned char initialCharacter, const TriePointer &childPointer);

    unsigned char checkRightmostCharWithLink(unsigned char initialCharacter, const TriePointer &childPointer);
};

} /* namespace Db */
#endif /* TRIENODE_H_ */
