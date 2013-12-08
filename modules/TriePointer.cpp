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
 * TriePointer.cpp
 *
 *  Created on: 10-12-2011
 *      Author: m
 */

#include "TriePointer.h"

namespace Db {

TriePointer::TriePointer() : leaf(0), link(0) {
}

TriePointer::TriePointer(bool pointee_is_leaf, unsigned long long int pointee_id)
    : leaf(pointee_is_leaf ? 1 : 0), link(pointee_id) {
}

bool TriePointer::operator ==(const TriePointer &pointer)
{
    return (pointer.leaf == leaf) && (pointer.link == link);
}

bool TriePointer::operator !=(const TriePointer &pointer)
{
    return (pointer.leaf != leaf) || (pointer.link != link);
}

bool TriePointer::isNull()
{
    return (leaf == 0) && (link == 0);
}

} /* namespace Db */
