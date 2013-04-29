#include <cstring>
#include <string>
#include <sstream>
#include <map>
#include <memory>
using namespace std;

#include "TriePointer.h"
#include "ObjectID.h"
#include "gtest/gtest.h"

namespace Db {
template <typename BinType>
class BinFile {
public:

    BinFile() : topID(1) {}

    BinType *getBin(unsigned long id) {
        return innerMap(id);
    }

    unsigned long long getNewBinByID() {
        unsigned long long id = topID++;
        innerMap[id] = new BinType();
        return id;
    }

    void freeBin(unsigned long id) {

    }

private:
    map<unsigned long, unique_ptr<BinType> > innerMap;
    unsigned long long topID;
};
}

#define BIN_FILE_TEST_OVERRIDE
#include "TrieNode.h"

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

TEST_F(TrieLeafTest, TestCheckRightmostCharWithLink)
{
    Db::TriePointer pointer(true, 12345);

    node.setChildrenRange(nodesFile, 40, 80, pointer);

    ASSERT_EQ(80, node.checkRightmostCharWithLink(nodesFile, 55, pointer));
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

TEST_F(TrieLeafTest, TestPointerIsNotTheOnlyNonNullField)
{
    Db::TriePointer pointer1(false, 12345);
    Db::TriePointer pointer2(false, 54321);

    node.setChildrenRange(nodesFile, 10, 20, pointer1);
    node.setChildrenRange(nodesFile, 50, 100, pointer2);

    ASSERT_FALSE(node.isPointerTheOnlyNonNullField(nodesFile, pointer1));
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
