/*
 * SystemParams.cpp
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#include "TrieNode.h"

#include "SystemParams.h"

#include <unistd.h>

namespace Db {

long SystemParams::_pageSize = 0;

#define INITIAL_NUMBER_OF_EMPTY_PAGES 16

long SystemParams::pageSize() {
    if (_pageSize == 0) {
        _pageSize = sysconf(_SC_PAGESIZE);
    }
    return _pageSize;
}

long SystemParams::initialIndexSize() {
    return minimalIndexExpandSize() * INITIAL_NUMBER_OF_EMPTY_PAGES;
}

long SystemParams::initialIndexMapSize() {
    /* one bit per trie node */
    return INITIAL_NUMBER_OF_EMPTY_PAGES / 8;
}

long SystemParams::minimalIndexExpandSize() {
    long pSize = pageSize();

    if (pSize < sizeof(TrieNode)) {
        if (sizeof(TrieNode) % pSize == 0) {
            return sizeof(TrieNode);
        } else {
            return pSize * ((sizeof(TrieNode) / pSize) + 1);
        }
    } else {
        return pSize;
    }
}

} /* namespace Db */
