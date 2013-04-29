
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
DatabaseKey.cpp
Slabs.cpp
""")

TEST_DIR = 'test'
GTEST_DIR = 'test/gtest-1.6.0/'
GMOCK_DIR = 'test/gmock-1.6.0/'

modules_full = ['modules/%s' % x for x in modules]

env = Environment()
#env['CCFLAGS'] = '-O2'
env['CCFLAGS'] = '-ggdb -std=c++11'

env.StaticLibrary(target='modules_lib', source=modules_full)

db_binary = env.Program(target='db', source=['main.cpp'], LIBS=['modules_lib', 'sqlite3', 'db_cxx-5.1'], LIBPATH='.')

Default(db_binary)

import re
import os.path as path

env.StaticLibrary(target='gtest_lib',
                  source=[GTEST_DIR + 'src/gtest-all.cc'],
                  CPPPATH=['modules', GTEST_DIR, GTEST_DIR + 'include'],
                  LINKFLAGS=['-pthread'])


from test import comparative

comparative_tests = [comparative.BerkeleyDBExecutable(),
                     comparative.SQLiteExecutable(),
                     comparative.OwnExecutable()]

# Prepare comparative tests targets (i.e. 'bdb_compare_test', 'skodb_compare_test', 'sqlite_compare_test').
# Such target actually builds a binary which later can be used to compare performance (etc.).
for test in comparative_tests:
    env.Program(target='%s_compare_test' % test.get_name(),
                source=['test/comparative_tests/main_%s.cpp' % test.get_name()],
                CPPPATH=['modules', 'test/comparative_tests'],
                LINKFLAGS=['-pthread'],
                LIBS=test.get_libs_list(), LIBPATH='.') 


class SpecialWish:

    def __init__(self, target_name, excludes_list):
        self._target_name = target_name
        self._excludes_list = excludes_list

    def get_target_name(self):
        return self._target_name

    def get_excludes_list(self):
        return self._excludes_list


special_wishes = [ SpecialWish('trie_node_test', ['modules/BinFile.cpp', 'modules/BinFileMap.cpp']) ]

special_wishes_map = {}
for wish in special_wishes:
    special_wishes_map[wish.get_target_name()] = wish

# Prepare unit tests targets (i.e. 'trie_leaf_test', 'trie_node_test').
# Such target builds a binary which tests a single module.
for module in modules:

    test_file_name = '%s/%s_test.cpp' % (TEST_DIR, module.rstrip('.cpp'))

    if not module.endswith('.cpp') or not path.isfile(test_file_name):
        continue

    base_module_name = re.sub('([A-Z])',
                              lambda l: '_' + l.group(1).lower(),
                              module.rstrip('.cpp')) \
                       .strip('_')

    modules_lib_name = 'modules_lib'

    target_name = base_module_name + '_test'

    if target_name in special_wishes_map:
        wish = special_wishes_map[target_name]
        modules_lib_name += '_' + base_module_name
        env.StaticLibrary(target=modules_lib_name, source=filter(lambda x: x not in wish.get_excludes_list(), modules_full))

    env.Program(target=target_name,
                source=[test_file_name],
                CPPPATH=['modules', GTEST_DIR, GTEST_DIR + 'include', GMOCK_DIR, GMOCK_DIR + 'include'],
                LINKFLAGS=['-pthread'],
                LIBS=[modules_lib_name, 'gtest_lib'], LIBPATH='.') 

