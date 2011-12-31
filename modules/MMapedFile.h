/*
 * MMapedFile.h
 *
 *  Created on: 06-12-2011
 *      Author: m
 */

#ifndef MMAPEDFILE_H_
#define MMAPEDFILE_H_

#include <string>

namespace Db {

class MMapedFile {
protected:
    int fd;
    unsigned long long int mmaped_size;
    void *fileStart;

public:

    enum OpeningResult {
        ERROR,
        NEW_FILE,
        OPENED,
    };

    OpeningResult openMMapedFile(const std::string &filename, unsigned long minimalInitialSize);

    void extendFileToSize(size_t newSize);

    void extendFileAndMmapingToSize(size_t newSize);

    std::string mmapErrnoToStr(int errnoNum);

    void closeMMapedFile();
};

} /* namespace Db */
#endif /* MMAPEDFILE_H_ */
