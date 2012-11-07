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
