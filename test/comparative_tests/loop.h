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
