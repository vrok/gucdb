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

SystemParams::SystemParams() {
    // TODO Auto-generated constructor stub

}

SystemParams::~SystemParams() {
    // TODO Auto-generated destructor stub
}

long SystemParams::pageSize() {
    if (_pageSize == 0) {
        _pageSize = sysconf(_SC_PAGESIZE);
    }
    return _pageSize;
}

long SystemParams::initialIndexSize() {
    return minimalExpandSize() * 16;
}

long SystemParams::minimalExpandSize() {
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
