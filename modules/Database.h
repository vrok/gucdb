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
public:
	Database(const string &dbDirectory);
	virtual ~Database();

	Value read(const DatabaseKey &key);
	int write(const DatabaseKey &key, const Value &value);
	int remove(const DatabaseKey &key);
	void dump();
};

} /* namespace Db */
#endif /* DATABASE_H_ */
