/*
 * Database.cpp
 *
 *  Created on: 27-11-2011
 *      Author: m
 */

#include <string>
#include <iostream>
using namespace std;

#include "Trie.h"
#include "BinFileMap.h"

#include "Database.h"

namespace Db {


Database::Database(const string & dbDirectory)
: dbDirectory(dbDirectory)
{

    mainIndex = new Trie(new BinFile<TrieNode>(dbDirectory + "/main.nodes",
                                               new BinFileMap(dbDirectory + "/main.nodes.map"),
                                               BinFile<TrieNode>::minimalIndexExpandSize() * 32),
                         new BinFile<TrieLeaf>(dbDirectory + "/main.leaves",
                                               new BinFileMap(dbDirectory + "/main.leaves.map"),
                                               BinFile<TrieLeaf>::minimalIndexExpandSize() * 8));
#if 0
	TrieNode *newNode = mainIndex->getNewBin();
	cout << "New node: " << newNode << endl;

	newNode = mainIndex->getNewBin();
    cout << "New node: " << newNode << endl;

    newNode = mainIndex->getNewBin();
    cout << "New node: " << newNode << endl;
#endif
    //newNode = mainIndex->getNewBin();
    //cout << "New node: " << newNode << endl;
}

Database::~Database()
{
    delete mainIndex;
}

char *Database::read(const char *key)
{
}

int Database::write(const char *key, const char *value)
{
}

} /* namespace Db */
