/*
 * Trie.cpp
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#include "SystemParams.h"
#include "TrieNode.h"

#include "Trie.h"

#include <iostream>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

namespace Db {

void Trie::extendFileToSize(off_t newSize) {
    int result = lseek(fd, newSize - 1, SEEK_SET);
    if (result == -1) {
        cerr << "lseek error" << endl;
    }
    result = write(fd, "", 1);
    if (result != 1) {
        cerr << "initial write error" << endl;
    }
}

void Trie::openTrieFile(const string & filename) {
    struct stat sb;
    off_t len;
    char *p;
    bool fillWithZerosAfterMmap = false;
    fd = open(filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        cerr << "open error" << endl;
    }
    if (fstat(fd, &sb) == -1) {
        cerr << "Fstat error" << endl;
    }
    if (!S_ISREG(sb.st_mode)) {
        cerr << filename << " is not a file" << endl;
    }

    mmaped_size = sb.st_size;

    if (sb.st_size < SystemParams::initialIndexSize()) {
        extendFileToSize(SystemParams::initialIndexSize());

        mmaped_size = SystemParams::initialIndexSize();
        fillWithZerosAfterMmap = true;
    }

    fileStart = mmap(0, mmaped_size, PROT_READ, MAP_SHARED, fd, 0);
    if (fileStart == MAP_FAILED) {
        int errorNum = errno;
        string errorDesc = mmapErrnoToStr(errorNum);
        cerr << "mmap failed with " << errorDesc << endl;
    }
    if (fillWithZerosAfterMmap) {
        cout << "Initial open, filling with zeros" << endl;
        for (unsigned long long *loc = (unsigned long long *) ((fileStart));
                (char*) ((loc)) < ((char*) ((fileStart)) + mmaped_size);
                loc++) {
            loc = 0;
        }
    }

    cout << "Size of TrieNode: " << sizeof(TrieNode) << endl;
    cout << "Page size: " << SystemParams::pageSize() << endl;
    cout << "Minimal expand size: " << SystemParams::minimalExpandSize() << endl;
}

Trie::Trie(const string & filename) {
    openTrieFile(filename);
    cout << "Index file opened" << endl;
}

string Trie::mmapErrnoToStr(int errnoNum) {
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

void Trie::closeTrieFile() {
    if (munmap(fileStart, mmaped_size) == -1) {
        cerr << "Error un-mapping trie file" << endl;
    }

    close(fd);
}

Trie::~Trie() {
    closeTrieFile();
}

} /* namespace Db */
