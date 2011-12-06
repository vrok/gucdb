/*
 * TrieMap.h
 *
 *  Created on: 06-12-2011
 *      Author: m
 */

#include "MMapedFile.h"

#include <string>
#include <queue>

#ifndef TRIEMAP_H_
#define TRIEMAP_H_

namespace Db {

class BinFileMap : public MMapedFile {
protected:
    std::queue<unsigned long> emptyBins;
    void loadMapCache();

public:
    BinFileMap(const std::string &filename);
    virtual ~BinFileMap();

    unsigned long fetchEmptyBin();
    void makeBinEmpty(unsigned long index);
};

} /* namespace Db */
#endif /* TRIEMAP_H_ */
