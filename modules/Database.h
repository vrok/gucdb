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
/*
 * Database.h
 *
 *  Created on: 27-11-2011
 *      Author: m
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <iostream>

namespace Db {

struct Value {
    const char * const data;
    const size_t dataSize;

    Value(const char *data, size_t dataSize)
        : data(data), dataSize(dataSize) {}

    /* WARNING: Resulting Value is as volatile as the passed argument.
     */
    Value(const std::string &str)
        : data(str.c_str()), dataSize(str.length()) {}

    bool isEmpty() {
        return NULL == data;
    }

    friend std::ostream & operator<<(std::ostream & stream, const Value &value) {
        stream.write(value.data, value.dataSize);
        return stream;
    }
};

class Database {
private:
	string dbDirectory;

	Trie<ObjectID> *mainIndex;
	Slabs *slabs;

	static const string emptyString;
public:
	Database(const string &dbDirectory);
	virtual ~Database();

	Value read(const DatabaseKey &key);
	int write(const DatabaseKey &key, const Value &value);
	int remove(const DatabaseKey &key);
	void dump();

	static const Value nullValue;
};

} /* namespace Db */
#endif /* DATABASE_H_ */
