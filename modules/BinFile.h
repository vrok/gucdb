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
    void assureBinIsMmaped(unsigned long id);

public: /* TODO: change to protected */
    unsigned long initialFileSize;
    string filename;

    BinFileMap *trieMap;

    OpeningResult openMMapedFile();

    BinType *getBin(unsigned long id);

    unsigned long long getNewBinByID();

    void freeBin(unsigned long id);

    static unsigned long minimalIndexExpandSize();

public:
    BinFile(const string &filename, BinFileMap *trieMap, unsigned long initialFileSize);
    ~BinFile();
};

} /* namespace Db */
#endif /* BINFILE_H_ */
