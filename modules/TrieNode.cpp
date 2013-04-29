/*
 * TrieNode.cpp
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#ifndef BIN_FILE_TEST_OVERRIDE
    // If defined, then a fake definition is provided by unittest
    #include "BinFile.h"
#endif

#include "TrieNode.h"
#include "Slabs.h"

#include <cstdlib>
#include <cstring>
#include <cassert>

namespace Db {

inline unsigned char getUpperHalf(unsigned char ch)
{
    return (ch & 0xf0) >> 4;
}

inline unsigned char getLowerHalf(unsigned char ch)
{
    return ch & 0x0f;
}

template<typename ValueType>
void TrieNode<ValueType>::setChildPointer(BinFile<TrieNode> &nodes,
        unsigned char character, const TriePointer &childPointer)
{
    memcpy(&children[character], &childPointer, sizeof(TriePointer));
}

template<typename ValueType>
void TrieNode<ValueType>::setChildrenRange(BinFile<TrieNode> &nodes,
        unsigned char firstCharacter, unsigned char lastCharacter,
        const TriePointer &childPointer)
{

    assert(firstCharacter <= lastCharacter);

    for (int character = firstCharacter; character <= lastCharacter; ++character) {
        /* We loop with int and then cast to uchar, because if we were setting the range [0, 255],
         * we would end with an infinite loop.
         */
        memcpy(&children[(unsigned char) character], &childPointer, sizeof(TriePointer));
    }
}

template<typename ValueType>
bool TrieNode<ValueType>::isLinkPure(BinFile<TrieNode> &nodes, unsigned char character)
{
    if (character == 0) {
        return children[character + 1] != children[character];
    } else
    if (character == (NODE_SIZE - 1)) {
        return children[character - 1] != children[character];
    } else {
        return (children[character - 1] != children[character]) &&
               (children[character + 1] != children[character]);
    }
}

template<typename ValueType>
unsigned char TrieNode<ValueType>::checkLeftmostCharWithLink(BinFile<TrieNode> &nodes, unsigned char initialCharacter,
                                                             const TriePointer &childPointer)
{
    unsigned char currentCharacter = initialCharacter;

    while (children[currentCharacter] == childPointer) {
        if (currentCharacter == 0) {
            return currentCharacter;
        }

        currentCharacter--;
    }

    return currentCharacter + 1;
}

template<typename ValueType>
unsigned char TrieNode<ValueType>::checkRightmostCharWithLink(BinFile<TrieNode> &nodes, unsigned char initialCharacter,
                                                              const TriePointer &childPointer)
{
    unsigned char currentCharacter = initialCharacter;

    while (children[currentCharacter] == childPointer) {
        if (currentCharacter == (NODE_SIZE - 1)) {
            return currentCharacter;
        }

        currentCharacter++;
    }

    return currentCharacter - 1;
}

template<typename ValueType>
bool TrieNode<ValueType>::isPointerTheOnlyNonNullField(BinFile<TrieNode> &nodes, const TriePointer &childPointer)
{
    for (int i = 0x00; i < NODE_SIZE; i++) {
        /* Yes, we check children and values in the same loop. */

        if (!children[i].isNull() && children[i] != childPointer) {
            return false;
        }

        if (values[i] != 0) {
            return false;
        }
    }

    return true;
}

//template class TrieNode<unsigned long long>;

template class TrieNode<ObjectID>;

} /* namespace Db */
