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
 * SystemParams.h
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#ifndef SYSTEMPARAMS_H_
#define SYSTEMPARAMS_H_

// We might prefer to use actual page size.
#define TYPICAL_PAGE_SIZE 4096

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
