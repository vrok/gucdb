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
