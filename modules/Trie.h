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
 * Trie.h
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#ifndef TRIE_H_
#define TRIE_H_

#include <string>

#include "BinFile.h"
#include "DatabaseKey.h"
#include "BinFileMap.h"
#include "TrieNode.h"
#include "TrieLeaf.h"

using namespace std;

namespace Db {

template <typename ValueType>
class Trie {
private:
    Trie(const Trie &);             /* Non-copiable */
    Trie & operator=(const Trie &); /* Non-copiable */

    BinFile<TrieNode<ValueType> > *nodes;
    BinFile<TrieLeaf<ValueType> > *leaves;

    bool deleteKeyAndJustDecideIfShouldCleanUpTrie(const DatabaseKey &key, vector<TrieNode<ValueType>*> &path);
    void cleanUpTrieAfterKeyRemoval(const DatabaseKey &key, const vector<TrieNode<ValueType>*> &pathToRemoved);

public:
	Trie(BinFile<TrieNode<ValueType> > *nodes, BinFile<TrieLeaf<ValueType> > *leaves);
	~Trie();

	void initializeEmpty();
    void addKey(const DatabaseKey &key, ValueType value);
    void deleteKey(const DatabaseKey &key);
    ValueType get(const DatabaseKey &key);
    void dump();
};

} /* namespace Db */
#endif /* TRIE_H_ */
