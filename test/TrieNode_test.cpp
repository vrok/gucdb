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
#include <cstring>
#include <string>
#include <sstream>
#include <map>
#include <memory>
#include <iostream>
using namespace std;

#include "TriePointer.h"
#include "ObjectID.h"
#include "gtest/gtest.h"

namespace Db {
template <typename BinType>
class BinFile {
public:

    BinFile() : topID(1) {}

    BinType *getBin(unsigned long id);

    unsigned long long getNewBinByID();

    void freeBin(unsigned long id);

private:
    map<unsigned long, unique_ptr<BinType> > innerMap;
    unsigned long long topID;
};

template<typename BinType>
BinType *BinFile<BinType>::getBin(unsigned long id) {
    return innerMap[id].get();
}

template<typename BinType>
unsigned long long BinFile<BinType>::getNewBinByID() {
    unsigned long long id = topID++;
    BinType *bt = new BinType();
    memset(bt, 0, sizeof(BinType));
    innerMap[id].reset(bt);
    return id;
}

template<typename BinType>
void BinFile<BinType>::freeBin(unsigned long id) {

}

}

#define BIN_FILE_TEST_OVERRIDE
#include "TrieNode.h"

namespace Db {
    template class BinFile<TrieNode<ObjectID> >;
}

namespace {

class TrieLeafTest: public ::testing::Test {
protected:

    typedef Db::TrieNode<Db::ObjectID> NodeType;

    NodeType node;
    Db::BinFile<NodeType> nodesFile;

    TrieLeafTest() {
    }

    virtual ~TrieLeafTest() {
    }

    virtual void SetUp() {
        memset((void*) &node, 0, sizeof(node));
    }

    virtual void TearDown() {
    }
};

TEST_F(TrieLeafTest, TestCheckLeftmostCharWithLink)
{
    Db::TriePointer pointer(true, 12345);

    node.setChildrenRange(nodesFile, 40, 80, pointer);

    ASSERT_EQ(40, node.checkLeftmostCharWithLink(nodesFile, 55, pointer));
}

TEST_F(TrieLeafTest, TestCheckLeftmostCharWithLink_LeftBound)
{
    Db::TriePointer pointer(true, 12345);

    node.setChildrenRange(nodesFile, 0, 80, pointer);

    ASSERT_EQ(0, node.checkLeftmostCharWithLink(nodesFile, 55, pointer));
}

TEST_F(TrieLeafTest, TestCheckRightmostCharWithLink)
{
    Db::TriePointer pointer(true, 12345);

    node.setChildrenRange(nodesFile, 40, 80, pointer);

    ASSERT_EQ(80, node.checkRightmostCharWithLink(nodesFile, 55, pointer));
}

TEST_F(TrieLeafTest, TestCheckRightmostCharWithLink_RightBound)
{
    Db::TriePointer pointer(true, 12345);

    node.setChildrenRange(nodesFile, 40, 0xff, pointer);

    ASSERT_EQ(0xff, node.checkRightmostCharWithLink(nodesFile, 55, pointer));
}

TEST_F(TrieLeafTest, TestCheckRightmostLeftmostBounds)
{
    Db::TriePointer pointer(true, 12345);

    node.setChildrenRange(nodesFile, 0, 0xff - 1, pointer);

    ASSERT_EQ(0, node.checkLeftmostCharWithLink(nodesFile, 55, pointer));
    ASSERT_EQ(0xff - 1, node.checkRightmostCharWithLink(nodesFile, 55, pointer));
}

TEST_F(TrieLeafTest, TestIsLinkPureLeftmost)
{
    Db::TriePointer pointer(true, 12345);

    node.setChildPointer(nodesFile, 0, pointer);
    ASSERT_TRUE(node.isLinkPure(nodesFile, 0));

    node.setChildPointer(nodesFile, 1, pointer);
    ASSERT_FALSE(node.isLinkPure(nodesFile, 0));
}

TEST_F(TrieLeafTest, TestIsLinkPureRightmost)
{
    Db::TriePointer pointer(true, 12345);

    node.setChildPointer(nodesFile, 0xff - 1, pointer);
    ASSERT_TRUE(node.isLinkPure(nodesFile, 0xff - 1));

    node.setChildPointer(nodesFile, 0xff - 2, pointer);
    ASSERT_FALSE(node.isLinkPure(nodesFile, 0xff - 1));
}

TEST_F(TrieLeafTest, TestIsLinkPureMiddleOnlyLeft)
{
    Db::TriePointer pointer(true, 12345);

    node.setChildPointer(nodesFile, 40, pointer);
    ASSERT_TRUE(node.isLinkPure(nodesFile, 40));

    node.setChildPointer(nodesFile, 39, pointer);
    ASSERT_FALSE(node.isLinkPure(nodesFile, 40));
}

TEST_F(TrieLeafTest, TestIsLinkPureMiddleOnlyRight)
{
    Db::TriePointer pointer(true, 12345);

    node.setChildPointer(nodesFile, 40, pointer);
    ASSERT_TRUE(node.isLinkPure(nodesFile, 40));

    node.setChildPointer(nodesFile, 41, pointer);
    ASSERT_FALSE(node.isLinkPure(nodesFile, 40));
}

TEST_F(TrieLeafTest, TestIsLinkPureMiddleBoth)
{
    Db::TriePointer pointer(true, 12345);

    node.setChildPointer(nodesFile, 40, pointer);
    ASSERT_TRUE(node.isLinkPure(nodesFile, 40));

    node.setChildrenRange(nodesFile, 39, 41, pointer);
    ASSERT_FALSE(node.isLinkPure(nodesFile, 40));
}

TEST_F(TrieLeafTest, TestPointerIsTheOnlyNonNullField)
{
    Db::TriePointer pointer(false, 12345);

    node.setChildrenRange(nodesFile, 10, 20, pointer);

    ASSERT_TRUE(node.isPointerTheOnlyNonNullField(nodesFile, pointer));
}

TEST_F(TrieLeafTest, TestPointerIsNotTheOnlyNonNullField_Sparse)
{
    Db::TriePointer pointer1(false, 12345);
    Db::TriePointer pointer2(false, 54321);

    node.setChildrenRange(nodesFile, 10, 20, pointer1);
    node.setChildrenRange(nodesFile, 50, 100, pointer2);

    ASSERT_FALSE(node.isPointerTheOnlyNonNullField(nodesFile, pointer1));
}

TEST_F(TrieLeafTest, TestPointerIsNotTheOnlyNonNullField_Dense)
{
    Db::TriePointer pointer(false, 12345);

    node.setChildrenRange(nodesFile, 0xa0, 0xaf, pointer);

    ASSERT_TRUE(node.isPointerTheOnlyNonNullField(nodesFile, pointer));
}

TEST_F(TrieLeafTest, TestPointerIsNotTheOnlyNonNullField_Dense2)
{
    Db::TriePointer pointer1(false, 12345);
    Db::TriePointer pointer2(false, 54321);

    node.setChildrenRange(nodesFile, 0xa0, 0xaf, pointer1);
    node.setChildrenRange(nodesFile, 0xb0, 0xbf, pointer2);

    ASSERT_FALSE(node.isPointerTheOnlyNonNullField(nodesFile, pointer1));
}

TEST_F(TrieLeafTest, TestSetGetValue)
{
    node.setValue(nodesFile, 'a', 1);
    node.setValue(nodesFile, 'b', 2);
    node.setValue(nodesFile, 'c', 3);

    ASSERT_EQ(1, node.getValue(nodesFile, 'a'));
    ASSERT_EQ(2, node.getValue(nodesFile, 'b'));
    ASSERT_EQ(3, node.getValue(nodesFile, 'c'));
}


TEST_F(TrieLeafTest, TestSetGetValue_WithOpsBetween)
{
    node.setValue(nodesFile, 'a', 1);
    node.setValue(nodesFile, 'b', 2);
    node.setValue(nodesFile, 0xff, 3);

    Db::TriePointer pointer(false, 12345);
    node.setChildrenRange(nodesFile, '0', 'a', pointer);
    node.setChildPointer(nodesFile, 'z', pointer);

    ASSERT_EQ(1, node.getValue(nodesFile, 'a'));
    ASSERT_EQ(2, node.getValue(nodesFile, 'b'));
    ASSERT_EQ(3, node.getValue(nodesFile, 0xff));
}

TEST_F(TrieLeafTest, TestSetGetAllValues_WithOpsBetween)
{
    for (int i = 0; i <= 0xff; i++) {
        node.setValue(nodesFile, static_cast<unsigned char>(i), i * 2);
    }

    Db::TriePointer pointer(false, 12345);
    node.setChildrenRange(nodesFile, '0', 'a', pointer);
    node.setChildPointer(nodesFile, 'z', pointer);

    for (int i = 0; i <= 0xff; i++) {
        ASSERT_EQ(i * 2, node.getValue(nodesFile, static_cast<unsigned char>(i)));
    }
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
