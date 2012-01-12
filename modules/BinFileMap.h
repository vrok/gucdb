/*
 * TrieMap.h
 *
 *  Created on: 06-12-2011
 *      Author: m
 */

#include "MMapedFile.h"

#include <string>
#include <queue>
#include <iterator>
#include <utility>


#ifndef TRIEMAP_H_
#define TRIEMAP_H_

namespace Db {

class BinFileMap : public MMapedFile {
public:
    class BinFileIterator : public std::iterator<std::forward_iterator_tag, std::pair<unsigned long, bool> >
    {
    private:
        friend class BinFileMap;
        BinFileMap &parent;

        unsigned int binId;
        BinFileIterator(BinFileMap &parent);
    public:
        BinFileIterator& operator++();
        bool operator==(const BinFileIterator &rhs);
        bool operator!=(const BinFileIterator &rhs);
        std::pair<unsigned long, bool> operator*();
    };

protected:
    std::queue<unsigned long> emptyBins;
    void loadMapCache();
    BinFileIterator endIterator;

public:
    BinFileMap(const std::string &filename);
    virtual ~BinFileMap();

    unsigned long fetchEmptyBin();
    void makeBinEmpty(unsigned long index);

    BinFileIterator getIterator();
    BinFileIterator& end();
};

} /* namespace Db */
#endif /* TRIEMAP_H_ */
