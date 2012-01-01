/*
 * Database.cpp
 *
 *  Created on: 27-11-2011
 *      Author: m
 */

#include <cstring>
#include <string>
#include <iostream>
#include <queue>
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
                                               BinFile<TrieLeaf>::minimalIndexExpandSize() * 1));
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

unsigned long long Database::read(const char *key)
{
    DatabaseKey dbKey;
    memcpy(dbKey.data, key, strlen(key));
    dbKey.length = strlen(key);

    return mainIndex->get(dbKey);
}

int Database::write(const char *key, unsigned long long value)
{
    DatabaseKey dbKey;
    //dbKey.data = key;
    memcpy(dbKey.data, key, strlen(key));
    dbKey.length = strlen(key);

    mainIndex->addKey(dbKey, value);
    return 0;
}

void Database::dump()
{
    mainIndex->dump();
}

} /* namespace Db */
