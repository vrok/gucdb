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
#include "SystemParams.h"

namespace Db {

const size_t NODE_SIZE = 0x0f + 1;

template<typename ValueType>
struct TrieNode {

    TrieNode() {}

    typedef ExponentialAllocator<TrieNode<ValueType>, TYPICAL_PAGE_SIZE / NODE_SIZE> AllocatorType;

    void setChildPointer(BinFile<TrieNode> &nodes,
                         unsigned char character, const TriePointer &childPointer);

    TriePointer &getChildPointer(BinFile<TrieNode> &nodes,
                                 unsigned char character);

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

    void setValue(BinFile<TrieNode> &nodes, unsigned char character,
                  ValueType &value);

    void setValue(BinFile<TrieNode> &nodes, unsigned char character,
                  ValueType &&value)
    {
        // This should be used just for simple types.
        ValueType v = value;
        setValue(nodes, character, v);
    }

    ValueType & getValue(BinFile<TrieNode> &nodes, unsigned char character);

private:

    void tryCaching(BinFile<TrieNode> &nodes, unsigned char character);
    bool areAnyValuesSet();

    template<typename Crawler>
    unsigned char checkXmostCharWithLink(BinFile<TrieNode> &nodes,
                                         unsigned char initialCharacter,
                                         const TriePointer &childPointer);

    TriePointer children[NODE_SIZE];

    union {
        ValueType values[NODE_SIZE];
        TriePointer grandChildrenCache[NODE_SIZE];
    };
};

} /* namespace Db */
#endif /* TRIENODE_H_ */
