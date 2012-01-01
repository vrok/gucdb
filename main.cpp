#include <iostream>
#include <string>
using namespace std;

#include "modules/Trie.h"
#include "modules/Database.h"

int main(int argc, char *argv[])
{
    string cmd, key;
    unsigned long long value;
    Db::Database dbInstance("/tmp/");
    while (1) {
        //cout << "> ";
        cout.flush();
        cin >> cmd;
        if (cmd == "read") {
            cin >> key;
            cout << dbInstance.read(key.c_str()) << endl;
        } else
        if (cmd == "write") {
            cin >> key >> value;
            cout << dbInstance.write(key.c_str(), value) << endl;
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
