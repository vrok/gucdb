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

    void setChildPointer(BinFile<TrieNode> &nodes,
                         unsigned char character, const TriePointer &childPointer);

    TriePointer &getChildPointer(BinFile<TrieNode> &nodes, unsigned char character)
    { return children[character]; } // Note that there are no safety checks here.

    void setChildrenRange(BinFile<TrieNode> &nodes, unsigned char firstCharacter,
                          unsigned char lastCharacter, const TriePointer &childPointer);

    bool isLinkPure(BinFile<TrieNode> &nodes, unsigned char character);

    unsigned char checkLeftmostCharWithLink(BinFile<TrieNode> &nodes,
                                            unsigned char initialCharacter,
                                            const TriePointer &childPointer);

    unsigned char checkRightmostCharWithLink(BinFile<TrieNode> &nodes,
                                             unsigned char initialCharacter,
                                             const TriePointer &childPointer);

    bool isPointerTheOnlyNonNullField(BinFile<TrieNode> &nodes,
                                      const TriePointer &childPointer);

    void setValue(BinFile<TrieNode> &nodes, unsigned char character, ValueType &value)
    { values[character] = value; }

    template<typename ConvertableToValueType>
    void setValue(BinFile<TrieNode> &nodes, unsigned char character, ConvertableToValueType value)
    { values[character] = value; }

    ValueType & getValue(BinFile<TrieNode> &nodes, unsigned char character)
    { return values[character]; }

private:
    TriePointer children[NODE_SIZE];

    union {
        ValueType values[NODE_SIZE];
        TriePointer grandChildrenCache[NODE_SIZE];
    };
};

} /* namespace Db */
#endif /* TRIENODE_H_ */
