#include <gtest/gtest.h>

#include <grammar/yy-misc.h>
#include <test/phony-errors.h>

#include "test-common.h"

using namespace test;

TEST(Syntax, ErrTabAsIndent)
{
    grammar::parse();
    ASSERT_TRUE(error::hasError());
    std::vector<TabAsIndentRec> recs = getTabAsIndentRecs();
    ASSERT_EQ(3, recs.size());

    EXPECT_EQ(misc::position(2), recs[0].pos);
    EXPECT_EQ(misc::position(4), recs[1].pos);
    EXPECT_EQ(misc::position(7), recs[2].pos);
}
