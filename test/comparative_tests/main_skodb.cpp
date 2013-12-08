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
#include <iostream>
#include <string>
#include <cassert>
using namespace std;

#include "loop.h"

#include "Trie.h"
#include "Slabs.h"
#include "Database.h"
#include "DatabaseKey.h"

class SkoDBAdapter
{
private:
    Db::Database dbInstance;
    Db::DatabaseKey dbKey;

public:

    SkoDBAdapter() : dbInstance("/tmp/") {
    }

    string read(const string & key) {
        dbKey = key;
        Db::Value val = dbInstance.read(dbKey);
        return string(val.data, val.dataSize);
    }

    void write(const string & key, const string & value) {
        dbKey = key;
        Db::Value dbValue(value);
        dbInstance.write(dbKey, dbValue);
    }

    void remove(const string & key) {
        dbKey = key;
        dbInstance.remove(dbKey);
    }

    void dump() {
        dbInstance.dump();
    }
};

int main(int argc, char *argv[])
{
    dbLoop<SkoDBAdapter>();
    return 0;
}

