#include <gtest/gtest.h>

#include <test/phony-errors.h>
#include <test/common.h>

#include "../yy-misc.h"

using namespace test;

TEST(Syntax, BadIndentation)
{
    yyparse();
    ASSERT_TRUE(error::hasError());
    std::vector<BadIndentRec> badIndRecs = getBadIndents();
    ASSERT_EQ(4, badIndRecs.size());

    EXPECT_EQ(misc::position(1), badIndRecs[0].pos);
    EXPECT_EQ(misc::position(2), badIndRecs[1].pos);
    EXPECT_EQ(misc::position(3), badIndRecs[2].pos);
    EXPECT_EQ(misc::position(5), badIndRecs[3].pos);
}
