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
	mainIndex = new Trie(dbDirectory + "/main.tidx", new BinFileMap(dbDirectory + "/main.map"));

	TrieNode *newNode = mainIndex->getNewBin();
	cout << "New node: " << newNode << endl;

	newNode = mainIndex->getNewBin();
    cout << "New node: " << newNode << endl;

    newNode = mainIndex->getNewBin();
    cout << "New node: " << newNode << endl;

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
