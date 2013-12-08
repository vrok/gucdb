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
#include <iostream>
#include <string>
#include <cassert>
using namespace std;

#include "loop.h"

#include <sqlite3.h>

class SQLiteAdapter
{
private:
    sqlite3 *db;
    sqlite3_stmt *read_stmt, *write_stmt, *remove_stmt;

    void perr() {
        cerr << "SQLite error: " << sqlite3_errmsg(db) << endl;
    }

    void query(std::string q) {
        sqlite3_stmt *create_stmt;
        assert(SQLITE_OK == sqlite3_prepare(db, q.c_str(), -1, &create_stmt, NULL));
        sqlite3_step(create_stmt);
        assert(SQLITE_OK == sqlite3_finalize(create_stmt));
    }

public:
    SQLiteAdapter() {
        assert(SQLITE_OK == sqlite3_open("/tmp/main.sqlite", &db));

        query("create table if not exists storage (k text, v text);");
        query("create index if not exists k_key on storage (k);");
        query("PRAGMA synchronous = OFF;");
        query("PRAGMA journal_mode = OFF;");
        query("PRAGMA locking_mode = EXCLUSIVE;");

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
        sqlite3_reset(remove_stmt);
        sqlite3_bind_text(remove_stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(remove_stmt);
    }

    void dump() {
        cerr << "SQLiteAdapter::dump() is unimplemented!";
    }
};

int main(int argc, char *argv[])
{
    dbLoop<SQLiteAdapter>();
    return 0;
}

