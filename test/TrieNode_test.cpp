#include <cstring>
#include <string>
#include <sstream>
using namespace std;

#include "TrieNode.h"
#include "TriePointer.h"
#include "gtest/gtest.h"

namespace {

class TrieLeafTest: public ::testing::Test {
protected:

    Db::TrieNode<unsigned long long> node;

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

    node.setChildrenRange(40, 80, pointer);

    ASSERT_EQ(40, node.checkLeftmostCharWithLink(55, pointer));
}

TEST_F(TrieLeafTest, TestCheckRightmostCharWithLink)
{
    Db::TriePointer pointer(true, 12345);

    node.setChildrenRange(40, 80, pointer);

    ASSERT_EQ(80, node.checkRightmostCharWithLink(55, pointer));
}

TEST_F(TrieLeafTest, TestCheckRightmostLeftmostBounds)
{
    Db::TriePointer pointer(true, 12345);

    node.setChildrenRange(0, 0xff - 1, pointer);

    ASSERT_EQ(0, node.checkLeftmostCharWithLink(55, pointer));
    ASSERT_EQ(0xff - 1, node.checkRightmostCharWithLink(55, pointer));
}

TEST_F(TrieLeafTest, TestIsLinkPureLeftmost)
{
    Db::TriePointer pointer(true, 12345);

    node.setChild(0, pointer);
    ASSERT_EQ(true, node.isLinkPure(0));

    node.setChild(1, pointer);
    ASSERT_EQ(false, node.isLinkPure(0));
}

TEST_F(TrieLeafTest, TestIsLinkPureRightmost)
{
    Db::TriePointer pointer(true, 12345);

    node.setChild(0xff - 1, pointer);
    ASSERT_EQ(true, node.isLinkPure(0xff - 1));

    node.setChild(0xff - 2, pointer);
    ASSERT_EQ(false, node.isLinkPure(0xff - 1));
}

TEST_F(TrieLeafTest, TestIsLinkPureMiddleOnlyLeft)
{
    Db::TriePointer pointer(true, 12345);

    node.setChild(40, pointer);
    ASSERT_EQ(true, node.isLinkPure(40));

    node.setChild(39, pointer);
    ASSERT_EQ(false, node.isLinkPure(40));
}

TEST_F(TrieLeafTest, TestIsLinkPureMiddleOnlyRight)
{
    Db::TriePointer pointer(true, 12345);

    node.setChild(40, pointer);
    ASSERT_EQ(true, node.isLinkPure(40));

    node.setChild(41, pointer);
    ASSERT_EQ(false, node.isLinkPure(40));
}

TEST_F(TrieLeafTest, TestIsLinkPureMiddleBoth)
{
    Db::TriePointer pointer(true, 12345);

    node.setChild(40, pointer);
    ASSERT_EQ(true, node.isLinkPure(40));

    node.setChildrenRange(39, 41, pointer);
    ASSERT_EQ(false, node.isLinkPure(40));
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
