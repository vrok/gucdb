/*
    Copyright 2013 Marcin Wrochniak

    This file is part of Guc DB.

    Guc DB is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    Guc DB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/
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

inline static unsigned char getUpperHalf(unsigned char ch)
{
    return (ch & 0xf0) >> 4;
}

inline static unsigned char getLowerHalf(unsigned char ch)
{
    return ch & 0x0f;
}

template<typename ValueType>
void TrieNode<ValueType>::tryCaching(BinFile<TrieNode> &nodes,
                                     unsigned char character)
{
    unsigned char upperHalf = getUpperHalf(character);

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
void TrieNode<ValueType>::setValue(BinFile<TrieNode> &nodes,
                                   unsigned char character,
                                   ValueType &value)
{
    unsigned char upperHalf = getUpperHalf(character);

    TriePointer &childID = children[upperHalf];

    if (childID.isNull()) {
        childID = TriePointer(false, nodes.getNewBinByID());
        grandChildrenCache[upperHalf] = TriePointer();
    }

    TrieNode *child = nodes.getBin(childID.link);
    child->values[getLowerHalf(character)] = value;

    if (value == static_cast<ValueType>(0)) {
        tryCaching(nodes, character);
    }
}

template<typename ValueType>
ValueType & TrieNode<ValueType>::getValue(BinFile<TrieNode> &nodes,
                                          unsigned char character)
{
    unsigned char upperHalf = getUpperHalf(character);

    TriePointer &childID = children[upperHalf];

    if (childID.isNull()) {
        static ValueType zero(0);
        return zero;
    }

    TrieNode *child = nodes.getBin(childID.link);
    return child->values[getLowerHalf(character)];
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
    unsigned char upperHalf = getUpperHalf(character);

    if (grandChildrenCache[upperHalf] == childPointer) {
        /* It's already set in the cache. */
        return;
    }

    unsigned char lowerHalf = getLowerHalf(character);

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
        tryCaching(nodes, character);
    }
}

template<typename ValueType>
TriePointer &TrieNode<ValueType>::getChildPointer(BinFile<TrieNode> &nodes,
                                                  unsigned char character)
{
    unsigned char upperHalf = getUpperHalf(character);
    unsigned char lowerHalf = getLowerHalf(character);

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

        unsigned char upperHalf = getUpperHalf(firstCharacter); // == getUpperHalf(lastCharacter)

        unsigned char from = getLowerHalf(firstCharacter);
        unsigned char to = getLowerHalf(lastCharacter);

        TriePointer childID = children[upperHalf];
        bool shouldTryCaching = true;

        if (childID.isNull()) {
            childID = TriePointer(false, nodes.getNewBinByID());
            children[upperHalf] = childID;

            if (from != 0x00 || to != 0xff) {
                shouldTryCaching = false;
            }
        }

        grandChildrenCache[upperHalf] = TriePointer();

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

        unsigned int fromNibble = getUpperHalf(firstCharacter) + 1;
        unsigned int toNibble = getUpperHalf(lastCharacter) - 1;

        for (unsigned int nibble = fromNibble; nibble <= toNibble; nibble++) {
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
}

template<typename ValueType>
bool TrieNode<ValueType>::isLinkPure(BinFile<TrieNode> &nodes, unsigned char character)
{
    if (character == 0) {
        return getChildPointer(nodes, character + 1) != getChildPointer(nodes, character);
    } else
    if (character == (NODE_SIZE - 1)) {
        return getChildPointer(nodes, character - 1) != getChildPointer(nodes, character);
    } else {
        return getChildPointer(nodes, character - 1) != getChildPointer(nodes, character) &&
               getChildPointer(nodes, character + 1) != getChildPointer(nodes, character);
    }
}

struct LeftCrawler
{
    int next(int character) { return character - 1; }
    int prev(int character) { return character + 1; }
    int wholeNibbleNext(int character) { return (character & 0xf0) - 1; }
    bool decLowerHalf(int &nibble) { return nibble-- > 0; }
    bool stillInRange(int character) { return character >= 0; }
};

struct RightCrawler
{
    int next(int character) { return character + 1; }
    int prev(int character) { return character - 1; }
    int wholeNibbleNext(int character) { return (character | 0x0f) + 1; }
    bool decLowerHalf(int &nibble) { return nibble++ < 0x0f; }
    bool stillInRange(int character) { return character <= 0xff; }
};

template<typename ValueType>
template<typename Crawler>
unsigned char TrieNode<ValueType>::checkXmostCharWithLink(BinFile<TrieNode> &nodes, unsigned char initialCharacter,
                                                          const TriePointer &childPointer)
{
    Crawler crawler;

    int currentCharacter = static_cast<int>(initialCharacter);

    while (crawler.stillInRange(currentCharacter)) {

        int upperHalf = static_cast<int>(getUpperHalf(currentCharacter));
        int lowerHalf = static_cast<int>(getLowerHalf(currentCharacter));

        TriePointer childID = children[upperHalf];

        if (children[upperHalf].isNull()) {
            if (grandChildrenCache[upperHalf] != childPointer)
                return crawler.prev(currentCharacter);

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

    return crawler.prev(currentCharacter);
}

template<typename ValueType>
unsigned char TrieNode<ValueType>::checkLeftmostCharWithLink(BinFile<TrieNode> &nodes, unsigned char initialCharacter,
                                                             const TriePointer &childPointer)
{
    return checkXmostCharWithLink<LeftCrawler>(nodes, initialCharacter, childPointer);
}

template<typename ValueType>
unsigned char TrieNode<ValueType>::checkRightmostCharWithLink(BinFile<TrieNode> &nodes, unsigned char initialCharacter,
                                                              const TriePointer &childPointer)
{
    return checkXmostCharWithLink<RightCrawler>(nodes, initialCharacter, childPointer);
}

template<typename ValueType>
bool TrieNode<ValueType>::isPointerTheOnlyNonNullField(BinFile<TrieNode> &nodes,
                                                       const TriePointer &childPointer)
{
    for (int i = 0x00; i < NODE_SIZE; i++) {
        TriePointer childID = children[i];

        if (childID.isNull()) {
            if (!grandChildrenCache[i].isNull() &&
                grandChildrenCache[i] != childPointer)
            {
                return false;
            }

            continue;
        }

        TrieNode *child = nodes.getBin(childID.link);

        for (int j = 0x00; j < NODE_SIZE; j++) {
            /* Yes, we check children and values in the same loop. */

            if (!child->children[j].isNull() && child->children[j] != childPointer) {
                return false;
            }

            if (child->values[j] != 0) {
                return false;
            }
        }
    }

    return true;
}

//template class TrieNode<unsigned long long>;

template class TrieNode<ObjectID>;

} /* namespace Db */
