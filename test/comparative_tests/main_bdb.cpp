#include <iostream>
#include <string>
#include <cassert>
using namespace std;

#include "loop.h"

#include <db_cxx.h>

class BDBAdapter
{
private:
    Db db;
    char tmpValue[1024];

public:

    BDBAdapter() : db(NULL, 0) {
        try {
            db.open(NULL, "/tmp/main.bdb", NULL, DB_BTREE, DB_CREATE, 0);               
        } catch(DbException &e) {
            cerr << "DbException" << endl;
        } catch(std::exception &e) {
            cerr << "std::exception" << endl;
        }
    }

    string read(const string & key) {
        Dbt bKey((char*) key.c_str(), key.size());
        Dbt bValue;
        bValue.set_data(tmpValue);
        bValue.set_ulen(sizeof(tmpValue));
        bValue.set_flags(DB_DBT_USERMEM);

        db.get(NULL, &bKey, &bValue, 0);
        return string(tmpValue, bValue.get_size());
    }

    void write(const string & key, const string & value) {
        Dbt bKey((char*) key.c_str(), key.size());
        Dbt bValue((char*) value.c_str(), value.size());
        db.put(NULL, &bKey, &bValue, 0); // DB_NOOVERWRITE
    }

    void remove(const string & key) {
        Dbt bKey((char*) key.c_str(), key.size());
        db.del(NULL, &bKey, 0);
    }

    void dump() {
        cerr << "BDBAdapter::dump() is unimplemented!";
    }
};

int main(int argc, char *argv[])
{
    dbLoop<BDBAdapter>();
    return 0;
}

