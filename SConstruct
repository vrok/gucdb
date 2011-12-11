
modules = Split("""
Database.cpp
TrieLeaf.cpp
TrieNode.cpp
Trie.cpp
SystemParams.cpp
MMapedFile.cpp
BinFileMap.cpp
BinFile.cpp
TriePointer.cpp
""")

TEST_DIR = 'test/'
GTEST_DIR = 'test/gtest-1.6.0/'

modules = ['modules/%s' % x for x in modules]

env = Environment()
env['CCFLAGS'] = '-ggdb'

db_binary = env.Program(target='db', source=['main.cpp'] + modules)

Default(db_binary)

trie_leaf_test = env.Program(target='trie_leaf_test',
                             source=['modules/TrieLeaf.cpp', 'modules/DatabaseKey.cpp', TEST_DIR + 'TrieLeaf_test.cpp', GTEST_DIR + 'src/gtest-all.cc'],
                             CPPPATH=['modules', GTEST_DIR, GTEST_DIR + 'include'],
                             LINKFLAGS=['-pthread']) 

