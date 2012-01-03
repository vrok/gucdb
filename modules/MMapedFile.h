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

namespace Db {

class MMapedFile {
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

    char* getOffsetLoc(off_t offset);

    void extendFileToSize(size_t newSize);

    bool extendFileAndMmapingToSize(size_t newSize);

    std::string mmapErrnoToStr(int errnoNum);

    void closeMMapedFile();
};

} /* namespace Db */
#endif /* MMAPEDFILE_H_ */
