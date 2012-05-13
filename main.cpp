#include <iostream>
#include <string>
using namespace std;

#include "modules/Trie.h"
#include "modules/Slabs.h"
#include "modules/Database.h"
#include "modules/DatabaseKey.h"

int main(int argc, char *argv[])
{
    string cmd, key, value;
    //unsigned long long value;
    Db::Database dbInstance("/tmp/");
    unsigned long long counter = 0;
    Db::DatabaseKey dbKey;
    //Db::Value dbValue;

    while (1) {
        if (counter++ == 2286) {
            //cout << counter << endl;
        }
        cout << "> ";
        cout.flush();
        cin >> cmd;
        if (cmd == "read") {
            cin >> key;
            //cin >> value;
            dbKey = key;
            cout << dbInstance.read(dbKey) << endl;
            //dbInstance.read(key.c_str());
        } else
        if (cmd == "write") {
            cin >> key >> value;

            dbKey = key;
            Db::Value dbValue(value);

            cout << dbInstance.write(dbKey, dbValue) << endl;
            //dbInstance.write(key.c_str(), value);
        } else
        if (cmd == "remove") {
            cin >> key;
            dbKey = key;
            cout << dbInstance.remove(dbKey) << endl;
            //dbInstance.remove(key.c_str());
        } else
        if (cmd == "dump") {
            dbInstance.dump();
        } else
        if (cmd == "exit") {
            return 0;
        } else {
            cerr << "Unknown command" << endl;
        }
    }
	return 0;
}

