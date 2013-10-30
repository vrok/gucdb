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

    DatabaseKey();

    DatabaseKey(unsigned char *data, size_t length) : length(length) {
        assert(length <= sizeof(this->data));
        memcpy(this->data, data, length);
    }

    ~DatabaseKey();

    void update(unsigned char *data, size_t length) {
        assert(length <= sizeof(this->data));
        this->length = length;
        memcpy(this->data, data, length);
    }

    DatabaseKey &operator=(const std::string & str) {
        assert(str.length() <= sizeof(this->data));

        memcpy(data, str.c_str(), str.length());
        length = str.length();

        return *this;
    }

    int getLength() const { return length; }

    void fillWithOneChar(unsigned char character) {
        data[0] = character;
        length = 1;
    }

    unsigned char operator [] (int index) const {
        return data[index];
    }

    const unsigned char* getData() const {
        return data;
    }

private:
    unsigned char data[512];
    int length;
};

} /* namespace Db */
#endif /* DATABASEKEY_H_ */
