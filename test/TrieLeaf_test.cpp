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
 * TrieLeaf_test.cpp
 *
 *  Created on: 11-12-2011
 *      Author: m
 */

#include <cstring>
#include <string>
#include <sstream>
using namespace std;

#include "BinFileAllocators.h"
#include "Slabs.h"
#include "TrieLeaf.h"
#include "gtest/gtest.h"

namespace {

class TrieLeafTest: public ::testing::Test {
protected:

    Db::TrieLeaf<Db::ObjectID> leaf;

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

    ASSERT_EQ(Db::ObjectID(0), leaf.get(key, 0));

    leaf.add(key, 0, 2357);

    ASSERT_EQ(2357, leaf.get(key, 0));
}

TEST_F(TrieLeafTest, TestGetSubstring)
{
    Db::DatabaseKey key1, key2;
    string key1_data = "test123";
    string key2_data = "ASDFtest123";

    memcpy((void*) key1.data, (void*) key1_data.c_str(), key1_data.length());
    memcpy((void*) key2.data, (void*) key2_data.c_str(), key2_data.length());
    key1.length = key1_data.length();
    key2.length = key2_data.length();

    leaf.add(key1, 0, 2357);

    ASSERT_EQ(2357, leaf.get(key1, 0));
    ASSERT_EQ(2357, leaf.get(key2, 4));
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

TEST_F(TrieLeafTest, TestUpdate)
{
    Db::DatabaseKey key;
    string key_data = "test123";
    memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
    key.length = key_data.length();

    leaf.add(key, 0, 3000);

    ASSERT_EQ(3000, leaf.get(key, 0));

    leaf.update(key, 0, 4000);

    ASSERT_EQ(4000, leaf.get(key, 0));
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
    ASSERT_TRUE(leaf.isEmpty());
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

    Db::TrieLeaf<Db::ObjectID> anotherLeaf;
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

    Db::TrieLeaf<Db::ObjectID> anotherLeaf;
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

    Db::TrieLeaf<Db::ObjectID> anotherLeaf;
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

TEST_F(TrieLeafTest, TestFindBestSplit)
{
    for (int i = 100; i <= 900; i += 100) {
        Db::DatabaseKey key;

        stringstream intToStringConverter;
        intToStringConverter << i;
        string key_data = intToStringConverter.str();

        memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
        key.length = key_data.length();

        leaf.add(key, 0, i);
    }

    ASSERT_EQ('5', leaf.findBestSplitPoint(0, 0xff));
}

TEST_F(TrieLeafTest, TestFindBestSplitFirstOccurenceOverwhelming)
{
    /* One character can occur as the first one so frequently, that the keys
     * it starts have size that's bigger than half of the leaf. Normally we wouldn't
     * include the first character exceeding half of the leaf size in the split,
     * but since it is the first one with any occurence, we do it.
     */
    string key_data("bolek");
    Db::DatabaseKey key;

    memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
    key.length = key_data.length();

    for (int i = 0; i < 20; i++) {
        leaf.add(key, 0, i);
    }

    key_data = "test";

    memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
    key.length = key_data.length();

    leaf.add(key, 0, 1234);

    ASSERT_EQ('c', leaf.findBestSplitPoint(0, 0xff));
}

TEST_F(TrieLeafTest, TestFindBestSplitLastOccurenceOverwhelming)
{
    string key_data("test");
    Db::DatabaseKey key;

    memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
    key.length = key_data.length();

    for (int i = 0; i < 20; i++) {
        leaf.add(key, 0, i);
    }

    key_data = "bolek";

    memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
    key.length = key_data.length();

    leaf.add(key, 0, 1234);

    ASSERT_EQ('t', leaf.findBestSplitPoint(0, 0xff));
}

TEST_F(TrieLeafTest, TestFindBestSplitOnlyOneInitialCharAndLeftBoundaryActivates)
{
    string key_data("test");
    Db::DatabaseKey key;

    memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
    key.length = key_data.length();

    for (int i = 0; i < 20; i++) {
        leaf.add(key, 0, i);
    }

    ASSERT_EQ('t', leaf.findBestSplitPoint(0, 't'));
}

TEST_F(TrieLeafTest, TestIsEmpty)
{
    string key_data("test");
    Db::DatabaseKey key;

    memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
    key.length = key_data.length();

    ASSERT_TRUE(leaf.isEmpty());

    leaf.add(key, 0, 1234);

    ASSERT_FALSE(leaf.isEmpty());
}

TEST_F(TrieLeafTest, TestStripOneLeadingCharacter)
{
    string key_data("test");
    Db::DatabaseKey key;

    memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
    key.length = key_data.length();

    leaf.add(key, 0, 1234);

    ASSERT_EQ(0, leaf.stripLeadingCharacter());

    ASSERT_EQ(0, leaf.get(key, 0));
    ASSERT_EQ(1234, leaf.get(key, 1));
}

TEST_F(TrieLeafTest, TestStripSingleLeadingCharacter)
{
    string key_data("t");
    Db::DatabaseKey key;

    memcpy((void*) key.data, (void*) key_data.c_str(), key_data.length());
    key.length = key_data.length();

    leaf.add(key, 0, 1234);

    ASSERT_EQ(1234, leaf.stripLeadingCharacter());

    ASSERT_TRUE(leaf.isEmpty());
}

TEST_F(TrieLeafTest, TestStripMultipleCharacters)
{
    Db::DatabaseKey key1, key2, key3;
    string key1_data = "abcd";
    string key2_data = "zzzzz";
    string key3_data = "e";

    memcpy((void*) key1.data, (void*) key1_data.c_str(), key1_data.length());
    memcpy((void*) key2.data, (void*) key2_data.c_str(), key2_data.length());
    memcpy((void*) key3.data, (void*) key3_data.c_str(), key3_data.length());

    key1.length = key1_data.length();
    key2.length = key2_data.length();
    key3.length = key3_data.length();

    leaf.add(key1, 0, 3000);
    leaf.add(key2, 0, 5000);
    leaf.add(key3, 0, 7000);

    ASSERT_EQ(7000, leaf.stripLeadingCharacter());

    ASSERT_EQ(3000, leaf.get(key1, 1));
    ASSERT_EQ(5000, leaf.get(key2, 1));
    ASSERT_EQ(0, leaf.get(key3, 1));
}

} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

