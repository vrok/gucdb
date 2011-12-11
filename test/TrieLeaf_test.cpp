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

// The fixture for testing class Foo.
class TrieLeafTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  TrieLeafTest() {
    // You can do set-up work for each test here.
  }

  virtual ~TrieLeafTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
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

#if 0
// Tests that the Foo::Bar() method does Abc.
TEST_F(FooTest, MethodBarDoesAbc) {
  const string input_filepath = "this/package/testdata/myinputfile.dat";
  const string output_filepath = "this/package/testdata/myoutputfile.dat";
  Foo f;
  EXPECT_EQ(0, f.Bar(input_filepath, output_filepath));
}

// Tests that Foo does Xyz.
TEST_F(FooTest, DoesXyz) {
  // Exercises the Xyz feature of Foo.
}
#endif

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

