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
 * MMapedFile.cpp
 *
 *  Created on: 06-12-2011
 *      Author: m
 */

#include "SystemParams.h"

#include "MMapedFile.h"

#include <map>
#include <algorithm>
#include <iostream>
using namespace std;

#include <cassert>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>


namespace Db {

void MMapedFile::extendFileToSize(size_t newSize) {
    int result = lseek(fd, newSize - 1, SEEK_SET);
    if (result == -1) {
        cerr << "lseek error" << endl;
    }
    result = write(fd, "", 1);
    if (result != 1) {
        cerr << "initial write error" << endl;
    }

}

bool MMapedFile::extendFileAndMmapingToSize(size_t newSize, bool shouldExtendFile) {
    if (shouldExtendFile) {
        extendFileToSize(newSize);
    }

    size_t newMmapingSize = newSize - mmaped_size;

    assert((newMmapingSize % SystemParams::pageSize()) == 0);

    void *mmapStart = mmap(0, newMmapingSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t) mmaped_size);
    if (mmapStart == MAP_FAILED) {
        int errorNum = errno;
        string errorDesc = mmapErrnoToStr(errorNum);
        cerr << ": mmap failed with " << errorDesc << endl;
        return false;
    }

    fileMmaps[mmaped_size] = (char*) mmapStart;
    mmaped_size = newSize;

    return true;
}

char* MMapedFile::getOffsetLoc(off_t offset) const {
    map<off_t, char*>::const_iterator desiredMmaping = --fileMmaps.upper_bound(offset);

    /* Calculate offset within the found mmaping. */
    off_t singleMmapingOffset = offset - desiredMmaping->first;

    return desiredMmaping->second + singleMmapingOffset;
}

MMapedFile::OpeningResult MMapedFile::openMMapedFile(const std::string &filename, size_t minimalInitialSize) {
    struct stat sb;

    fd = open(filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        cerr << "open error" << endl;
        return ERROR;
    }

    if (fstat(fd, &sb) == -1) {
        cerr << "Fstat error" << endl;
        return ERROR;
    }

    if (!S_ISREG(sb.st_mode)) {
        cerr << filename << " is not a file" << endl;
        return ERROR;
    }

    mmaped_size = 0;

    size_t initialSize = minimalInitialSize;
    bool fileExteningNeeded = true;

    size_t stSizeNotAsOffset = static_cast<size_t>(sb.st_size);
    if (stSizeNotAsOffset >= initialSize) {
        initialSize = stSizeNotAsOffset;
        fileExteningNeeded = false;
    }

    extendFileAndMmapingToSize(initialSize, fileExteningNeeded);

    if (stSizeNotAsOffset < minimalInitialSize) {
        return NEW_FILE;
    }

    return OPENED;
}

string MMapedFile::mmapErrnoToStr(int errnoNum) const {
    switch(errno) {
        case EACCES: return "EACCES";
        case EAGAIN: return "EAGAIN";
        case EBADF: return "EBADF";
        case EINVAL: return "EINVAL";
        case ENFILE: return "ENFILE";
        case ENODEV: return "ENODEV";
        case ENOMEM: return "ENOMEM";
        case EOVERFLOW: return "EOVERFLOW";
        case EPERM: return "EPERM";
        default: return "Unknown";
    }
}

void MMapedFile::closeMMapedFile() {
#if 0
    if (munmap(fileStart, mmaped_size) == -1) {
        cerr << "Error un-mapping trie file" << endl;
    }
#endif
    close(fd);
}


} /* namespace Db */
