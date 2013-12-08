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
 * MMapedFile.h
 *
 *  Created on: 06-12-2011
 *      Author: m
 */

#ifndef MMAPEDFILE_H_
#define MMAPEDFILE_H_

#include <cstddef>
#include <string>
#include <map>

#include <sys/types.h>

namespace Db {

class MMapedFile {
private:
    MMapedFile & operator=(const MMapedFile &); /* Non-copiable */

protected:
    int fd;
    size_t mmaped_size;
    std::map<off_t, char*> fileMmaps;

public:

    enum OpeningResult {
        ERROR,
        NEW_FILE,
        OPENED,
    };

    OpeningResult openMMapedFile(const std::string &filename, size_t minimalInitialSize);

    char* getOffsetLoc(off_t offset) const;

    void extendFileToSize(size_t newSize);

    bool extendFileAndMmapingToSize(size_t newSize, bool shouldExtendFile = true);

    std::string mmapErrnoToStr(int errnoNum) const;

    void closeMMapedFile();
};

} /* namespace Db */
#endif /* MMAPEDFILE_H_ */
