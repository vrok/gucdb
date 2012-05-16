#include <iostream>
#include <string>
using namespace std;

#include "modules/Trie.h"
#include "modules/Slabs.h"
#include "modules/Database.h"
#include "modules/DatabaseKey.h"



class Adapter
{
private:
    Db::Database dbInstance;
    Db::DatabaseKey dbKey;

public:

    Adapter() : dbInstance("/tmp/") {
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
};

template<typename A>
void dbLoop() {
    A a;
    string cmd, key, value;

    while (1) {
        cout << "> ";
        cout.flush();
        cin >> cmd;
        if (cmd == "read") {
            cin >> key;
            cout << a.read(key) << endl;
        } else
        if (cmd == "write") {
            cin >> key >> value;
            a.write(key, value);
        } else
        if (cmd == "remove") {
            cin >> key;
            a.remove(key);
        } else
        if (cmd == "exit") {
            return;
        } else {
            cerr << "Unknown command" << endl;
        }
    }
}


int main(int argc, char *argv[])
{
    dbLoop<Adapter>();
}

