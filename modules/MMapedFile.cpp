/*
 * MMapedFile.cpp
 *
 *  Created on: 06-12-2011
 *      Author: m
 */

#include "SystemParams.h"

#include "MMapedFile.h"

#include <iostream>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

// TODO: CDT goes crazy without it, possibly remove it in future
#ifndef size_t
//#define size_t long unsigned int
#endif

namespace Db {

void MMapedFile::extendFileToSize(off_t newSize) {
    int result = lseek(fd, newSize - 1, SEEK_SET);
    if (result == -1) {
        cerr << "lseek error" << endl;
    }
    result = write(fd, "", 1);
    if (result != 1) {
        cerr << "initial write error" << endl;
    }
}

MMapedFile::OpeningResult MMapedFile::openMMapedFile(const std::string &filename, unsigned long minimalInitialSize) {
    struct stat sb;
    off_t len;
    char *p;
    bool fillWithZerosAfterMmap = false;

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

    mmaped_size = sb.st_size;

    if (sb.st_size < minimalInitialSize) {
        extendFileToSize(minimalInitialSize);

        mmaped_size = minimalInitialSize;
        fillWithZerosAfterMmap = true;
    }

    fileStart = mmap(0, mmaped_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fileStart == MAP_FAILED) {
        int errorNum = errno;
        string errorDesc = mmapErrnoToStr(errorNum);
        cerr << filename << ": mmap failed with " << errorDesc << endl;
        return ERROR;
    }

    if (fillWithZerosAfterMmap) {
        cout << "Initial open, filling with zeros" << endl;
        char *fileEnd = ((char*)fileStart) + mmaped_size;
        for (char *loc = (char*)(fileStart); loc < fileEnd; loc++) {
            *loc = 0;
        }

        return NEW_FILE;
    }

    return OPENED;
}

string MMapedFile::mmapErrnoToStr(int errnoNum) {
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
    if (munmap(fileStart, mmaped_size) == -1) {
        cerr << "Error un-mapping trie file" << endl;
    }

    close(fd);
}


} /* namespace Db */
