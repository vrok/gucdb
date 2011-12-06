/*
 * SystemParams.h
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#ifndef SYSTEMPARAMS_H_
#define SYSTEMPARAMS_H_

namespace Db {

class SystemParams {
private:
    static long _pageSize;
public:
    static long pageSize();

    static long initialIndexSize();

    static long initialIndexMapSize();

    static long minimalIndexExpandSize();
};

} /* namespace Db */
#endif /* SYSTEMPARAMS_H_ */
