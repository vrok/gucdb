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

unsigned char TrieNode::checkLeftmostCharWithLink(unsigned char initialCharacter, const TriePointer &childPointer)
{
    unsigned char currentCharacter = initialCharacter;

    while (children[currentCharacter] == childPointer) {
        currentCharacter--;

        if (currentCharacter == 0)
            return currentCharacter;
    }

    return currentCharacter++;
}

unsigned char TrieNode::checkRightmostCharWithLink(unsigned char initialCharacter, const TriePointer &childPointer)
{
    unsigned char currentCharacter = initialCharacter;

    while (children[currentCharacter] == childPointer) {
        currentCharacter++;

        if (currentCharacter == (NODE_SIZE - 1))
            return currentCharacter;
    }

    return currentCharacter--;
}

} /* namespace Db */
