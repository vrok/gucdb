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
#include "Slabs.h"

#include "Database.h"

namespace Db {

const string Database::emptyString = "";
const Value Database::nullValue(emptyString);

Database::Database(const string & dbDirectory)
: dbDirectory(dbDirectory)
{

    mainIndex = new Trie<ObjectID>(new BinFile<TrieNode<ObjectID> >(dbDirectory + "/main.nodes",
                                               new BinFileMap(dbDirectory + "/main.nodes.map"),
                                               BinFile<TrieNode<ObjectID> >::minimalIndexExpandSize() * 32),
                                       new BinFile<TrieLeaf<ObjectID> >(dbDirectory + "/main.leaves",
                                               new BinFileMap(dbDirectory + "/main.leaves.map"),
                                               BinFile<TrieLeaf<ObjectID> >::minimalIndexExpandSize() * 1));

    slabs = new Slabs(new BinFile<Slab>(dbDirectory + "/main.slabs",
                                        new BinFileMap(dbDirectory + "/main.slabs.map"),
                                        BinFile<Slab>::minimalIndexExpandSize()),
                      new BinFile<SlabInfo>(dbDirectory + "/main.slabinfos",
                                            new BinFileMap(dbDirectory + "/main.slabinfos.map"),
                                            BinFile<SlabInfo>::minimalIndexExpandSize()));


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

Value Database::read(const DatabaseKey &key)
{
    ObjectID oid = mainIndex->get(key);

    if (oid.isNull())
        return nullValue;

    char *result = NULL;
    size_t resultSize = slabs->readData(result, oid);

    return Value(result, resultSize);
}

int Database::write(const DatabaseKey &key, const Value &value)
{
    ObjectID oid = slabs->saveData(value.data, value.dataSize);
    mainIndex->addKey(key, oid);
    return 0;
}

int Database::remove(const DatabaseKey &key)
{
    ObjectID oid = mainIndex->get(key);
    if (! oid.isNull()) {
        mainIndex->deleteKey(key);
        slabs->removeData(oid);
    }
    return 0;
}

void Database::dump()
{
    mainIndex->dump();
    //slabs->dumpAllInfo(cout);
}

} /* namespace Db */
