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

class Trie {
private:
    BinFile<TrieNode> *nodes;
    BinFile<TrieLeaf> *leaves;

public:
	Trie(BinFile<TrieNode> *nodes, BinFile<TrieLeaf> *leaves);
	~Trie();

	void initializeEmpty();
	void addKey(const DatabaseKey &key, unsigned long long value);
};

} /* namespace Db */
#endif /* TRIE_H_ */
