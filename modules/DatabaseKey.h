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

    DatabaseKey(unsigned char *data, int length) : length(length) {
        assert(length <= sizeof(data));
        memcpy(this->data, data, length);
    }

    ~DatabaseKey();

    void update(unsigned char *data, int length) {
        assert(length <= sizeof(this->data));
        this->length = length;
        memcpy(this->data, data, length);
    }

    DatabaseKey * operator=(const std::string & str) {
        assert(str.length() <= sizeof(this->data));

        memcpy(data, str.c_str(), str.length());
        length = str.length();
    }
};

} /* namespace Db */
#endif /* DATABASEKEY_H_ */
