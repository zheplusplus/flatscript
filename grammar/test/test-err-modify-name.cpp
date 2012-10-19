#include <gtest/gtest.h>

#include <test/phony-errors.h>

#include "test-common.h"
#include "../yy-misc.h"

using namespace test;

TEST(Syntax, ModifyName)
{
    yyparse();
    ASSERT_TRUE(error::hasError());
    std::vector<ModifyNameRec> recs = getModifyNameRecs();
    ASSERT_EQ(1, recs.size());

    EXPECT_EQ(misc::position(2), recs[0].pos);
    EXPECT_EQ("x", recs[0].name);
}
