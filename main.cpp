#include <iostream>
#include <string>
using namespace std;

#include "modules/Trie.h"
#include "modules/Database.h"

#if 0
int main(int argc, char *argv[])
{
    string cmd, key;
    unsigned long long value;
    Db::Database dbInstance("/tmp/");
    unsigned long long counter = 0;
    while (1) {
        if (counter++ % 1000 == 0) {
            cout << counter << endl;
        }
        //cout << "> ";
        cout.flush();
        cin >> cmd;
        if (cmd == "read") {
            cin >> key;
            cout << dbInstance.read(key.c_str()) << endl;
            //dbInstance.read(key.c_str());
        } else
        if (cmd == "write") {
            cin >> key >> value;
            //cout << dbInstance.write(key.c_str(), value) << endl;
            dbInstance.write(key.c_str(), value);
        } else
        if (cmd == "remove") {
            cin >> key;
            //cout << dbInstance.remove(key.c_str()) << endl;
            dbInstance.remove(key.c_str());
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
#endif

