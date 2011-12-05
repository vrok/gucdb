#include <iostream>
#include <string>
using namespace std;

#include "modules/Trie.h"
#include "modules/Database.h"

int main(int argc, char *argv[])
{
    string cmd, key, value;
    Db::Database dbInstance("/tmp/");
    while (1) {
        cin >> cmd;
        if (cmd == "read") {
            cin >> key;
            cout << dbInstance.read(key.c_str());
        } else
        if (cmd == "write") {
            cin >> key >> value;
            cout << dbInstance.write(key.c_str(), value.c_str());
        } else {
            cerr << "Unknown command" << endl;
        }
    }
	return 0;
}
