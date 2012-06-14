/*
 * BinFile.h
 *
 *  Created on: 06-12-2011
 *      Author: m
 */

#include <string>
using namespace std;

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
