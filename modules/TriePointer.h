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
 * TriePointer.h
 *
 *  Created on: 10-12-2011
 *      Author: m
 */

#ifndef TRIEPOINTER_H_
#define TRIEPOINTER_H_

namespace Db {

struct TriePointer {
    TriePointer();
    TriePointer(bool pointee_is_leaf, unsigned long long int pointee_id);

    unsigned char leaf : 1;
    unsigned long long int link : 63;

    bool operator== (const TriePointer &pointer);
    bool operator!= (const TriePointer &pointer);
    bool isNull();
};

} /* namespace Db */
#endif /* TRIEPOINTER_H_ */
