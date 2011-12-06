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
#include "BinFileMap.h"
#include "TrieNode.h"

using namespace std;

namespace Db {

class Trie : public BinFile<TrieNode> {
public:
	Trie(const string &filename, BinFileMap *trieMap);
	virtual ~Trie();
};

} /* namespace Db */
#endif /* TRIE_H_ */
