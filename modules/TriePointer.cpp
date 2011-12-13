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

} /* namespace Db */
