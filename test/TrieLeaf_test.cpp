/*
 * TrieLeaf_test.cpp
 *
 *  Created on: 11-12-2011
 *      Author: m
 */

#include <cstring>

#include "TrieLeaf.h"
#include "gtest/gtest.h"

namespace {

class TrieLeafTest : public ::testing::Test {
protected:

  TrieLeafTest() {
  }

  virtual ~TrieLeafTest() {
  }

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

TEST_F(TrieLeafTest, TestGetAddGet)
{
    Db::TrieLeaf leaf;
    memset((void*) &leaf, 0, sizeof(leaf));

    Db::DatabaseKey key;
    char *key_data = "test123";

    memcpy((void*) key.data, (void*) key_data, strlen(key_data));
    key.length = strlen(key_data);

    ASSERT_EQ(0, leaf.get(key, 0));

    leaf.add(key, 0, 2357);

    ASSERT_EQ(2357, leaf.get(key, 0));
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

