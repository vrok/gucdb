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
 * ExponentialBinFileAllocator.h
 *
 *  Created on: Jun 14, 2012
 *      Author: m
 */

#ifndef BINFILEALLOCATORS_H_
#define BINFILEALLOCATORS_H_

#include <cstdlib>

/* Double the size of the next mmapping (with a limit).
 * Efficiency is a one reason, the other (probably more important)
 * is that usually there's a limit to how many mmappings a process
 * in a system can have. It could be easily hit during a large DB import, etc.
 */
template<typename BinType, int firstAllocationFactor = 1>
class ExponentialAllocator
{
private:
    size_t nextAllocationSize;

public:
    ExponentialAllocator()
        : nextAllocationSize(sizeof(BinType) * firstAllocationFactor) {}

    size_t getNextAllocationSize() {
        if (nextAllocationSize < (sizeof(BinType) * 128)) {
            size_t result = nextAllocationSize;
            nextAllocationSize *= 2;
            return result;
        }

        return nextAllocationSize;
    }
};


template<typename BinType>
class LinearAllocator
{
public:
    size_t getNextAllocationSize() {
        return sizeof(BinType);
    }
};

#endif /* BINFILEALLOCATORS_H_ */
