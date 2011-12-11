/*
 * TrieLeaf_test.cpp
 *
 *  Created on: 11-12-2011
 *      Author: m
 */

#include <cstring>
#include <string>
using namespace std;

#include "TrieLeaf.h"
#include "gtest/gtest.h"

namespace {

class TrieLeafTest: public ::testing::Test {
protected:

    Db::TrieLeaf leaf;

    TrieLeafTest() {
    }

    virtual ~TrieLeafTest() {
    }

    virtual void SetUp() {
        memset((void*) &leaf, 0, sizeof(leaf));
    }

    virtual void TearDown() {
    }
};

TEST_F(TrieLeafTest, TestSimpleGetAddGet)
{
    Db::DatabaseKey key;
    string key_data = "test123";

    memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
    key.length = key_data.length();

    ASSERT_EQ(0, leaf.get(key, 0));

    leaf.add(key, 0, 2357);

    ASSERT_EQ(2357, leaf.get(key, 0));
}

TEST_F(TrieLeafTest, TestSomeGetsAndSets)
{
    Db::DatabaseKey key1, key2;
    string key1_data = "test123";
    string key2_data = "tere fere dutki";

    memcpy((void*) key1.data, (void*) key1_data.c_str(), key1_data.length());
    memcpy((void*) key2.data, (void*) key2_data.c_str(), key2_data.length());

    key1.length = key1_data.length();
    key2.length = key2_data.length();

    ASSERT_EQ(0, leaf.get(key1, 0));
    ASSERT_EQ(0, leaf.get(key2, 0));

    leaf.add(key1, 0, 3000);

    ASSERT_EQ(3000, leaf.get(key1, 0));
    ASSERT_EQ(0, leaf.get(key2, 0));

    leaf.add(key2, 0, 5000);

    ASSERT_EQ(3000, leaf.get(key1, 0));
    ASSERT_EQ(5000, leaf.get(key2, 0));
}

TEST_F(TrieLeafTest, TestRemove)
{
    Db::DatabaseKey key;
    string key_data = "test123";
    memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
    key.length = key_data.length();

    leaf.add(key, 0, 3000);

    ASSERT_EQ(3000, leaf.get(key, 0));

    leaf.remove(key, 0);

    ASSERT_EQ(0, leaf.get(key, 0));
}

TEST_F(TrieLeafTest, TestRemoveTwo)
{
    Db::DatabaseKey key1, key2;
    string key1_data = "test123";
    string key2_data = "tere fere dutki";

    memcpy((void*) key1.data, (void*) key1_data.c_str(), key1_data.length());
    memcpy((void*) key2.data, (void*) key2_data.c_str(), key2_data.length());

    key1.length = key1_data.length();
    key2.length = key2_data.length();

    leaf.add(key1, 0, 3000);
    leaf.add(key2, 0, 5000);

    ASSERT_EQ(3000, leaf.get(key1, 0));
    ASSERT_EQ(5000, leaf.get(key2, 0));

    leaf.remove(key1, 0);

    ASSERT_EQ(0, leaf.get(key1, 0));
    ASSERT_EQ(5000, leaf.get(key2, 0));

    leaf.remove(key2, 0);

    ASSERT_EQ(0, leaf.get(key1, 0));
    ASSERT_EQ(0, leaf.get(key2, 0));
}


} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

