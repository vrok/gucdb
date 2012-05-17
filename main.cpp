#include <iostream>
#include <string>
using namespace std;

#include "modules/Trie.h"
#include "modules/Slabs.h"
#include "modules/Database.h"
#include "modules/DatabaseKey.h"

#include <sqlite3.h>


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

class SQLiteAdapter
{
private:
    sqlite3 *db;
    sqlite3_stmt *read_stmt, *write_stmt, *remove_stmt;

    void perr() {
        cerr << "SQLite error: " << sqlite3_errmsg(db) << endl;
    }

public:
    SQLiteAdapter() {
        assert(SQLITE_OK == sqlite3_open("/tmp/main.sqlite", &db));

        sqlite3_stmt *create_stmt;
        assert(SQLITE_OK == sqlite3_prepare(db, "create table if not exists storage (k string, v string);", -1, &create_stmt, NULL));
        sqlite3_step(create_stmt);
        assert(SQLITE_OK == sqlite3_finalize(create_stmt));

        assert(SQLITE_OK == sqlite3_prepare(db, "create index if not exists k_key on storage (k);", -1, &create_stmt, NULL));
        sqlite3_step(create_stmt);
        assert(SQLITE_OK == sqlite3_finalize(create_stmt));

        assert(SQLITE_OK == sqlite3_prepare(db, "select v from storage where k = ?;", -1, &read_stmt, NULL));
        assert(SQLITE_OK == sqlite3_prepare(db, "insert into storage (k, v) values (?, ?);", -1, &write_stmt, NULL));
        assert(SQLITE_OK == sqlite3_prepare(db, "delete from storage where k = ?;", -1, &remove_stmt, NULL));
    }

    string read(const string & key) {
        sqlite3_reset(read_stmt);
        sqlite3_bind_text(read_stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(read_stmt);
        const char * result = (const char*) sqlite3_column_text(read_stmt, 0);
        if (NULL == result)
            return "";
        return string(result);
    }

    void write(const string & key, const string & value) {
        sqlite3_reset(write_stmt);
        sqlite3_bind_text(write_stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(write_stmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(write_stmt);
    }

    void remove(const string & key) {
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
    //dbLoop<Adapter>();
    dbLoop<SQLiteAdapter>();
}

