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
    SystemParams();
    virtual ~SystemParams();

    static long pageSize();

    static long initialIndexSize();

    static long minimalExpandSize();
};

} /* namespace Db */
#endif /* SYSTEMPARAMS_H_ */
