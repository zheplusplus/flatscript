#include <gtest/gtest.h>

#include <test/phony-errors.h>

#include "test-common.h"
#include "../yy-misc.h"

using namespace test;

TEST(Syntax, BadIndentation)
{
    yyparse();
    ASSERT_TRUE(error::hasError());
    std::vector<InvalidIndentRec> recs(getInvalidIndentRecs());
    ASSERT_EQ(3, recs.size());

    EXPECT_EQ(misc::position(3), recs[0].pos);
    EXPECT_EQ(misc::position(5), recs[1].pos);
    EXPECT_EQ(misc::position(8), recs[2].pos);
}
