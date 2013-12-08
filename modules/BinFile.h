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
 * BinFile.h
 *
 *  Created on: 06-12-2011
 *      Author: m
 */

#include <string>
using namespace std;

#include <sys/types.h>

#include "MMapedFile.h"
#include "BinFileMap.h"

#ifndef BINFILE_H_
#define BINFILE_H_

namespace Db {

template <typename BinType>
class BinFile : public MMapedFile {
private:
    void assureNewBinIsUsable(off_t binOffset);
    off_t getBinOffset(unsigned long id);

    BinFileMap *binFileMap;

    typename BinType::AllocatorType allocator;

public: /* TODO: change to protected */
    unsigned long initialFileSize;
    string filename;

    OpeningResult openMMapedFile();

    BinType *getBin(unsigned long id);
    unsigned long long getNewBinByID();
    bool isBinFree(unsigned long id);
    bool isBinIDSafeAndAllocated(unsigned long id);
    void freeBin(unsigned long id);

    static unsigned long minimalIndexExpandSize();

    const BinFileMap & getBinFileMap();

public:
    BinFile(const string &filename, BinFileMap *trieMap, unsigned long initialFileSize);
    ~BinFile();
};

} /* namespace Db */
#endif /* BINFILE_H_ */
