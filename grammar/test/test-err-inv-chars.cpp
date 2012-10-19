#include <gtest/gtest.h>

#include <test/phony-errors.h>

#include "test-common.h"
#include "../yy-misc.h"

using namespace test;

TEST(Syntax, InvalidCharacters)
{
    yyparse();
    ASSERT_TRUE(error::hasError());
    std::vector<InvCharRec> recs = getInvCharRecs();
    ASSERT_EQ(3, recs.size());

    EXPECT_EQ(misc::position(1), recs[0].pos);
    EXPECT_EQ(int('`'), recs[0].character);
    EXPECT_EQ(misc::position(2), recs[1].pos);
    EXPECT_EQ(int('?'), recs[1].character);
    EXPECT_EQ(misc::position(4), recs[2].pos);
    EXPECT_EQ(int(';'), recs[2].character);
}
