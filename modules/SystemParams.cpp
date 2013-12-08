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
 * SystemParams.cpp
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

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

#if 0
long SystemParams::initialIndexSize() {
    return minimalIndexExpandSize() * INITIAL_NUMBER_OF_EMPTY_PAGES;
}
#endif

long SystemParams::initialIndexMapSize() {
    /* one bit per trie node */
    return INITIAL_NUMBER_OF_EMPTY_PAGES / 8;
}

#if 0
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
#endif

} /* namespace Db */
