
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

modules = ['modules/%s' % x for x in modules]

env = Environment()
env['CCFLAGS'] = '-ggdb'

env.Program(target='db', source=['main.cpp'] + modules)

