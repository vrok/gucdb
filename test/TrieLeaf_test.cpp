/*
 * TrieLeaf_test.cpp
 *
 *  Created on: 11-12-2011
 *      Author: m
 */

#include <cstring>
#include <string>
#include <sstream>
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

TEST_F(TrieLeafTest, TestEmptyKey)
{
    Db::DatabaseKey key;
    string key_data = "";

    memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
    key.length = key_data.length();

    ASSERT_EQ(0, leaf.get(key, 0));

    leaf.add(key, 0, 2357);

    ASSERT_EQ(2357, leaf.get(key, 0));
}

TEST_F(TrieLeafTest, TestMoveToAnother)
{
    Db::DatabaseKey key1, key2;
    string key1_data = "abcd";
    string key2_data = "zzzzz";

    memcpy((void*) key1.data, (void*) key1_data.c_str(), key1_data.length());
    memcpy((void*) key2.data, (void*) key2_data.c_str(), key2_data.length());

    key1.length = key1_data.length();
    key2.length = key2_data.length();

    leaf.add(key1, 0, 3000);
    leaf.add(key2, 0, 5000);

    ASSERT_EQ(3000, leaf.get(key1, 0));
    ASSERT_EQ(5000, leaf.get(key2, 0));

    Db::TrieLeaf anotherLeaf;
    memset((void*) &anotherLeaf, 0, sizeof(anotherLeaf));

    Db::DatabaseKey dividing_key;
    string dividing_data = "test";
    memcpy((void*) dividing_key.data, (void*) dividing_data.c_str(), dividing_data.length());
    dividing_key.length = dividing_data.length();

    leaf.moveAllBelowToAnotherLeaf(dividing_key, 0, anotherLeaf);

    ASSERT_EQ(0, leaf.get(key1, 0));
    ASSERT_EQ(5000, leaf.get(key2, 0));

    ASSERT_EQ(3000, anotherLeaf.get(key1, 0));
    ASSERT_EQ(0, anotherLeaf.get(key2, 0));
}

TEST_F(TrieLeafTest, TestMoveToAnotherDivideByExisting)
{
    /* Key equal to the divider should not be moved. */

    Db::DatabaseKey key;
    string key1_data = "abcd";

    memcpy((void*) key.data, (void*) key1_data.c_str(), key1_data.length());

    key.length = key1_data.length();

    leaf.add(key, 0, 3000);

    ASSERT_EQ(3000, leaf.get(key, 0));

    Db::TrieLeaf anotherLeaf;
    memset((void*) &anotherLeaf, 0, sizeof(anotherLeaf));

    Db::DatabaseKey dividing_key;
    string dividing_data = "abcd";
    memcpy((void*) dividing_key.data, (void*) dividing_data.c_str(), dividing_data.length());
    dividing_key.length = dividing_data.length();

    leaf.moveAllBelowToAnotherLeaf(dividing_key, 0, anotherLeaf);

    ASSERT_EQ(3000, leaf.get(key, 0));

    ASSERT_EQ(0, anotherLeaf.get(key, 0));
}

TEST_F(TrieLeafTest, TestBulkMoveToAnother)
{
    for (int i = 1; i < 40; i++) {
        Db::DatabaseKey key;

        stringstream intToStringConverter;
        intToStringConverter << i;

        string key_data = string("test") + intToStringConverter.str();
        memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
        key.length = key_data.length();

        leaf.add(key, 0, i);
    }

    Db::TrieLeaf anotherLeaf;
    memset((void*) &anotherLeaf, 0, sizeof(anotherLeaf));

    Db::DatabaseKey dividing_key;
    string dividing_data = "test1";
    memcpy((void*) dividing_key.data, (void*) dividing_data.c_str(), dividing_data.length());
    dividing_key.length = dividing_data.length();

    leaf.moveAllBelowToAnotherLeaf(dividing_key, 0, anotherLeaf);


    for (int i = 1; i < 40; i++) {
        Db::DatabaseKey key;

        stringstream intToStringConverter;
        intToStringConverter << i;

        string key_data = string("test") + intToStringConverter.str();
        memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
        key.length = key_data.length();

        if (dividing_data <= key_data) {
            ASSERT_EQ(i, leaf.get(key, 0));
            ASSERT_EQ(0, anotherLeaf.get(key, 0));
        } else {
            ASSERT_EQ(0, leaf.get(key, 0));
            ASSERT_EQ(i, anotherLeaf.get(key, 0));
        }
    }
}


} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

