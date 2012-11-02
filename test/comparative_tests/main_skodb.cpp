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

