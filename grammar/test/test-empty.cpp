#include <gtest/gtest.h>

#include <test/phony-errors.h>

#include "test-common.h"
#include "../yy-misc.h"

TEST(Syntax, Empty)
{
    grammar::parse();
    ASSERT_FALSE(error::hasError());
}
