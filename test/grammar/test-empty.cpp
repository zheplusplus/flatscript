#include <gtest/gtest.h>

#include <grammar/yy-misc.h>
#include <test/phony-errors.h>

#include "test-common.h"

TEST(Syntax, Empty)
{
    grammar::parse();
    ASSERT_FALSE(error::hasError());
}
