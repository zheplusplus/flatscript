#include <gtest/gtest.h>

#include <grammar/yy-misc.h>
#include <test/phony-errors.h>

#include "test-common.h"

using namespace test;

TEST(Syntax, ReservedWords)
{
    grammar::parse();
    ASSERT_TRUE(error::hasError());
    std::vector<ReservedWordRec> recs(getReservedWordRecs());
    ASSERT_EQ(4, recs.size());

    EXPECT_EQ(misc::position(1), recs[0].pos);
    EXPECT_EQ("_", recs[0].token);
    EXPECT_EQ(misc::position(1), recs[1].pos);
    EXPECT_EQ("X", recs[1].token);
    EXPECT_EQ(misc::position(2), recs[2].pos);
    EXPECT_EQ("__", recs[2].token);
    EXPECT_EQ(misc::position(2), recs[3].pos);
    EXPECT_EQ("A", recs[3].token);
}
