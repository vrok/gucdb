/*
 * DatabaseKey.h
 *
 *  Created on: 10-12-2011
 *      Author: m
 */

#ifndef DATABASEKEY_H_
#define DATABASEKEY_H_

#include <string>
#include <cassert>
#include <cstring>

namespace Db {

class DatabaseKey {
public:
    unsigned char data[512];
    int length;

    DatabaseKey();
    ~DatabaseKey();

    DatabaseKey * operator=(const std::string & str) {
        assert(str.length() <= sizeof(data));

        memcpy(data, str.c_str(), str.length());
        length = str.length();
    }
};

} /* namespace Db */
#endif /* DATABASEKEY_H_ */
