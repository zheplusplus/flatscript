#include <gtest/gtest.h>

#include <test/phony-errors.h>

#include "test-common.h"
#include "../yy-misc.h"

using namespace test;

TEST(Syntax, SliceStepOmitted)
{
    grammar::parse();
    ASSERT_TRUE(error::hasError());
    std::vector<SliceStepOmittedRec> recs = getSliceStepOmittedRecs();
    ASSERT_EQ(1, recs.size());

    EXPECT_EQ(misc::position(5), recs[0].pos);
}
