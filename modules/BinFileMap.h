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
    class Iterator : public std::iterator<std::forward_iterator_tag, std::pair<unsigned long, bool> >
    {
    private:
        friend class BinFileMap;
        const BinFileMap &parent;

        unsigned int binId;
        Iterator(const BinFileMap &parent);
    public:
        Iterator& operator++();
        bool operator==(const Iterator &rhs);
        bool operator!=(const Iterator &rhs);
        std::pair<unsigned long, bool> operator*();
    };

protected:
    std::queue<unsigned long> emptyBins;
    void loadMapCache();
    mutable Iterator endIterator;

public:
    BinFileMap(const std::string &filename);
    virtual ~BinFileMap();

    unsigned long fetchEmptyBin();
    void makeBinEmpty(unsigned long index);
    bool isBinEmpty(unsigned long index);

    Iterator getIterator() const;
    Iterator& end() const;
};

} /* namespace Db */
#endif /* TRIEMAP_H_ */
