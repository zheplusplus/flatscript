#ifndef __FLSC_GRAMMAR_TEST_SAMPLES_TEST_H__
#define __FLSC_GRAMMAR_TEST_SAMPLES_TEST_H__

#include <gtest/gtest.h>
#include <util/io.h>
#include <grammar/syntax.h>
#include <test/phony-errors.h>

#include "test-common.h"

namespace test {

    struct SyntaxSampleTest
        : GrammarTest
    {
        void parseSampleOk(std::string const& file)
        {
            auto result(grammar::build("", util::read_file(file)));
            ASSERT_FALSE(error::hasError());
            result->compile(nulScope());
        }

        void parseSampleErr(std::string const& file)
        {
            grammar::build("", util::read_file(file));
            ASSERT_TRUE(error::hasError());
        }
    };

}

#endif /* __FLSC_GRAMMAR_TEST_SAMPLES_TEST_H__ */
