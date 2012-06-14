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
template<typename BinType>
class ExponentialAllocator
{
private:
    size_t nextAllocationSize;

public:
    ExponentialAllocator() : nextAllocationSize(sizeof(BinType)) {}

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
