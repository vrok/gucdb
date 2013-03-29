/*
 * TrieNode.h
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#ifndef TRIENODE_H_
#define TRIENODE_H_

#include "TriePointer.h"
#include "BinFileAllocators.h"

namespace Db {

const size_t NODE_SIZE = 0xff + 1;

template<typename ValueType>
struct TrieNode {
    typedef ExponentialAllocator<TrieNode<ValueType> > AllocatorType;

    void setChildPointer(unsigned char character, const TriePointer &childPointer);

    TriePointer &getChildPointer(unsigned char character)
    { return children[character]; } // Note that there are no safety checks here.

    void setChildrenRange(unsigned char firstCharacter, unsigned char lastCharacter, const TriePointer &childPointer);

    bool isLinkPure(unsigned char character);

    unsigned char checkLeftmostCharWithLink(unsigned char initialCharacter, const TriePointer &childPointer);

    unsigned char checkRightmostCharWithLink(unsigned char initialCharacter, const TriePointer &childPointer);

    bool isPointerTheOnlyNonNullField(const TriePointer &childPointer);

    void setValue(unsigned char character, ValueType &value)
    { values[character] = value; }

    template<typename ConvertableToValueType>
    void setValue(unsigned char character, ConvertableToValueType value)
    { values[character] = value; }

    ValueType & getValue(unsigned char character)
    { return values[character]; }

private:
    TriePointer children[NODE_SIZE];
    ValueType values[NODE_SIZE];
};

} /* namespace Db */
#endif /* TRIENODE_H_ */
