/*
 * Database.h
 *
 *  Created on: 27-11-2011
 *      Author: m
 */

#ifndef DATABASE_H_
#define DATABASE_H_

namespace Db {

class Database {
private:
	string dbDirectory;

	Trie *mainIndex;
public:
	Database(const string &dbDirectory);
	virtual ~Database();

	unsigned long long read(const char *key);
	int write(const char *key, unsigned long long value);
	void dump();
};

} /* namespace Db */
#endif /* DATABASE_H_ */
