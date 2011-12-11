/*
 * TrieNode.cpp
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#include "TrieNode.h"

#include <cstdlib>
#include <cstring>
#include <cassert>

namespace Db {

void TrieNode::setChild(unsigned char character,
        const TriePointer &childPointer) {
    memcpy(&children[character], &childPointer, sizeof(TriePointer));
}

void TrieNode::setChildrenRange(unsigned char firstCharacter,
        unsigned char lastCharacter, const TriePointer &childPointer) {

    assert(firstCharacter <= lastCharacter);

    for (unsigned char character = firstCharacter; character <= lastCharacter; ++character) {
        memcpy(&children[character], &childPointer, sizeof(TriePointer));
    }
}

bool TrieNode::isLinkPure(unsigned char character)
{
    if (character == 0) {
        return children[character + 1] != children[character];
    } else
    if (character == (NODE_SIZE - 1)) {
        return children[character - 1] != children[character];
    } else {
        return (children[character - 1] != children[character]) && (children[character + 1] != children[character]);
    }
}

}

/* namespace Db */
