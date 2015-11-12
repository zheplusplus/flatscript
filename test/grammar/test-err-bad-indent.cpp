#include <gtest/gtest.h>

#include <grammar/yy-misc.h>
#include <test/phony-errors.h>

#include "test-common.h"

using namespace test;

TEST(Syntax, BadIndentation)
{
    grammar::parse();
    ASSERT_TRUE(error::hasError());
    std::vector<InvalidIndentRec> recs(getInvalidIndentRecs());
    ASSERT_EQ(4, recs.size());

    EXPECT_EQ(misc::position(3), recs[0].pos);
    EXPECT_EQ(misc::position(5), recs[1].pos);
    EXPECT_EQ(misc::position(6), recs[2].pos);
    EXPECT_EQ(misc::position(8), recs[3].pos);
}
