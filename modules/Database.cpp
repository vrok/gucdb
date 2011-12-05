/*
 * Database.cpp
 *
 *  Created on: 27-11-2011
 *      Author: m
 */

#include <string>
using namespace std;

#include "Trie.h"

#include "Database.h"

namespace Db {


Database::Database(const string & dbDirectory)
: dbDirectory(dbDirectory)
{
	mainIndex = new Trie(dbDirectory + "/main.tidx");
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
