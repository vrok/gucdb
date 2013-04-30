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
#include <algorithm>

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
void TrieNode<ValueType>::tryCaching(BinFile<TrieNode> &nodes, char character)
{
    char upperHalf = getUpperHalf(character);

    TriePointer &childID = children[upperHalf];
    TrieNode *child = nodes.getBin(childID.link);

    TriePointer sampleGrandchildPointer = child->children[0];

    for (int i = 0; i < NODE_SIZE; i++) {
        if (child->children[i] != sampleGrandchildPointer || child->values[i] != 0) {
            /* Caching is not possible. */
            return;
        }
    }

    /* We can get by with just the parent node: all links in the child are the same,
     * no values are set, so we can cache the link value in the parent.
     */
    nodes.freeBin(childID.leaf);
    children[upperHalf] = TriePointer(false, 0);
    grandChildrenCache[upperHalf] = sampleGrandchildPointer;
}

template<typename ValueType>
bool TrieNode<ValueType>::areAnyValuesSet()
{
    for (int i = 0; i < NODE_SIZE; i++) {
        if (values[i] != 0) {
            return true;
        }
    }
    return false;
}

template<typename ValueType>
void TrieNode<ValueType>::setChildPointer(BinFile<TrieNode> &nodes,
        unsigned char character, const TriePointer &childPointer)
{
    char upperHalf = getUpperHalf(character);

    if (grandChildrenCache[upperHalf] == childPointer) {
        /* It's already set in the cache. */
        return;
    }

    char lowerHalf = getLowerHalf(character);

    TriePointer childID = children[upperHalf];
    bool shouldTryCaching = true;

    if (childID.isNull()) {
        childID = TriePointer(false, nodes.getNewBinByID());
        children[upperHalf] = childID;
        shouldTryCaching = false;
    }

    assert(0 == children[upperHalf].leaf);

    TrieNode *child = nodes.getBin(childID.link);
    child->children[lowerHalf] = childPointer;

    if (shouldTryCaching) {
        //for (int i = 0; i < NODE_SIZE; i++) {
        //    if (child->children[i] != childPointer || child->values[i] != 0) {
        //        return;
        //    }
        //}
        //nodes.freeBin(childID.leaf);
        //children[upperHalf] = TriePointer(false, 0);
        //grandChildrenCache[upperHalf] = childPointer;
        tryCaching(nodes, character);
    }
    //memcpy(&children[character], &childPointer, sizeof(TriePointer));
}

template<typename ValueType>
TriePointer &TrieNode<ValueType>::getChildPointer(BinFile<TrieNode> &nodes,
                                                  unsigned char character)
{
    char upperHalf = getUpperHalf(character);
    char lowerHalf = getLowerHalf(character);

    if (children[upperHalf].isNull()) {
        return grandChildrenCache[upperHalf];
    } else {
        TrieNode *child = nodes.getBin(children[upperHalf].link);
        return child->children[lowerHalf];
    }
}

template<typename ValueType>
void TrieNode<ValueType>::setChildrenRange(BinFile<TrieNode> &nodes,
        unsigned char firstCharacter, unsigned char lastCharacter,
        const TriePointer &childPointer)
{

    assert(firstCharacter <= lastCharacter);

    if (getUpperHalf(firstCharacter) == getUpperHalf(lastCharacter)) {

        char upperHalf = getUpperHalf(firstCharacter); // == getUpperHalf(lastCharacter)

        char from = getLowerHalf(firstCharacter);
        char to = getLowerHalf(lastCharacter);

        TriePointer childID = children[upperHalf];
        bool shouldTryCaching = true;

        if (childID.isNull()) {
            childID = TriePointer(false, nodes.getNewBinByID());
            children[upperHalf] = childID;

            if (from != 0x00 || to != 0xff) {
                shouldTryCaching = false;
            }
        }

        assert(0 == children[upperHalf].leaf);

        TrieNode *child = nodes.getBin(childID.link);

        for (int character = from; character <= to; character++) {
            child->children[character] = childPointer;
        }

        if (shouldTryCaching) {
            tryCaching(nodes, firstCharacter);
        }
    } else {
        setChildrenRange(nodes, firstCharacter, (getUpperHalf(firstCharacter) << 4) | 0x0f, childPointer);

        char fromNibble = getUpperHalf(firstCharacter) + 1;
        char toNibble = getUpperHalf(lastCharacter) - 1;

        for (char nibble = fromNibble; nibble <= toNibble; nibble++) {
            TriePointer &pointer = children[nibble];

            if (! pointer.isNull()) {
                TrieNode *childNode = nodes.getBin(children[nibble].link);
                assert(childNode != NULL);
                if (childNode->areAnyValuesSet()) {
                    fill(childNode->children, childNode->children + NODE_SIZE, childPointer);
                    continue;
                }

                nodes.freeBin(children[nibble].link);
                children[nibble] = TriePointer(false, 0);
            }

            grandChildrenCache[nibble] = childPointer;
        }

        setChildrenRange(nodes, getUpperHalf(lastCharacter) << 4, lastCharacter, childPointer);
    }

#if 0
    for (int character = firstCharacter; character <= lastCharacter; ++character) {
        /* We loop with int and then cast to uchar, because if we were
         * setting the range [0, 255], we would end with an infinite loop.
         */
        memcpy(&children[(unsigned char) character], &childPointer, sizeof(TriePointer));
    }
#endif
}

template<typename ValueType>
bool TrieNode<ValueType>::isLinkPure(BinFile<TrieNode> &nodes, unsigned char character)
{
    if (character == 0) {
        //return children[character + 1] != children[character];
        return getChildPointer(nodes, character + 1) != getChildPointer(nodes, character);
    } else
    if (character == (NODE_SIZE - 1)) {
        //return children[character - 1] != children[character];
        return getChildPointer(nodes, character - 1) != getChildPointer(nodes, character);
    } else {
        return getChildPointer(nodes, character - 1) != getChildPointer(nodes, character) &&
               getChildPointer(nodes, character + 1) != getChildPointer(nodes, character);
        //return (children[character - 1] != children[character]) &&
        //       (children[character + 1] != children[character]);
    }
}

struct LeftCrawler
{
    int nextUpperHalf(int nibble) { return nibble - 1; }
    int firstLowerHalf() { return 0x0f; }
    int next(int character) { return character - 1; }
    int prev(int character) { return character + 1; }
    int wholeNibbleNext(int character) { return (character & 0xf0) - 1; }
    bool decLowerHalf(int &nibble) { return nibble-- > 0; }
};

struct RightCrawler
{
    int nextUpperHalf(int nibble) { return nibble + 1; }
    int firstLowerHalf() { return 0x00; }
    int next(int character) { return character + 1; }
    int prev(int character) { return character - 1; }
    int wholeNibbleNext(int character) { return (character | 0x0f) + 1; }
    bool decLowerHalf(int &nibble) { return nibble++ < 0x0f; }
};

template<typename ValueType>
template<typename Crawler>
unsigned char TrieNode<ValueType>::checkXmostCharWithLink(BinFile<TrieNode> &nodes, unsigned char initialCharacter,
                                                          const TriePointer &childPointer)
{
    Crawler crawler;

    int currentCharacter = static_cast<int>(initialCharacter);

    int upperHalf = static_cast<int>(getUpperHalf(currentCharacter));
    int lowerHalf = static_cast<int>(getLowerHalf(currentCharacter));

    while (currentCharacter >= 0) {
        TriePointer childID = children[upperHalf];

        if (children[upperHalf].isNull()) {
            if (grandChildrenCache[upperHalf] != childPointer)
                return crawler.prev(currentCharacter);

            upperHalf = crawler.nextUpperHalf(upperHalf);
            lowerHalf = crawler.firstLowerHalf();
            currentCharacter = crawler.wholeNibbleNext(currentCharacter);
        } else {
            TrieNode *childNode = nodes.getBin(childID.link);
            do {
                if (childNode->children[lowerHalf] != childPointer) {
                    return crawler.prev(currentCharacter);
                }
                currentCharacter = crawler.next(currentCharacter);
            } while (crawler.decLowerHalf(lowerHalf));
        }
    }
}

template<typename ValueType>
unsigned char TrieNode<ValueType>::checkLeftmostCharWithLink(BinFile<TrieNode> &nodes, unsigned char initialCharacter,
                                                             const TriePointer &childPointer)
{
    checkXmostCharWithLink<LeftCrawler>(nodes, initialCharacter, childPointer);
}

template<typename ValueType>
unsigned char TrieNode<ValueType>::checkRightmostCharWithLink(BinFile<TrieNode> &nodes, unsigned char initialCharacter,
                                                              const TriePointer &childPointer)
{
    checkXmostCharWithLink<RightCrawler>(nodes, initialCharacter, childPointer);
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
