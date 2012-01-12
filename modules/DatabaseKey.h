/*
 * DatabaseKey.h
 *
 *  Created on: 10-12-2011
 *      Author: m
 */

#ifndef DATABASEKEY_H_
#define DATABASEKEY_H_

namespace Db {

class DatabaseKey {
public:
    unsigned char data[512];
    int length;

    DatabaseKey();
    ~DatabaseKey();
};

} /* namespace Db */
#endif /* DATABASEKEY_H_ */
