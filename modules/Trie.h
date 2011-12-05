/*
 * Trie.h
 *
 *  Created on: 05-12-2011
 *      Author: m
 */

#ifndef TRIE_H_
#define TRIE_H_

#include <string>
using namespace std;

namespace Db {

class Trie {
private:
    int fd;
    unsigned long long int mmaped_size;
    void *fileStart;

    string mmapErrnoToStr(int errnoNum);
    void openTrieFile(const string & filename);
    void closeTrieFile();
    void extendFileToSize(off_t newSize);
public:
	Trie(const string &filename);
	virtual ~Trie();
};

} /* namespace Db */
#endif /* TRIE_H_ */
