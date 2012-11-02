#ifndef _LOOP_H_
#define _LOOP_H_

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
        } else
        if (cmd == "dump") {
            a.dump();
        } else {
            cerr << "Unknown command: " << cmd << endl;
        }
    }
}

#endif /* _LOOP_H_ */
