#include <gtest/gtest.h>

#include "../string.h"

TEST(String, ReplaceAll)
{
    ASSERT_EQ("aa", util::replace_all("aa", "aaa", "bbb"));
    ASSERT_EQ("@@", util::replace_all("@@", "@", "@"));
    ASSERT_EQ("$$", util::replace_all("$", "$", "$$"));
    ASSERT_EQ("$#$#", util::replace_all("$$", "$", "$#"));
    ASSERT_EQ("$#$#", util::replace_all("##", "#", "$#"));
}

TEST(String, FromInt)
{
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(std::string(1, char('0' + i)), util::str(i));
    }
    ASSERT_EQ("1024", util::str(1024));

    for (int i = -9; i < 0; ++i) {
        ASSERT_EQ("-" + std::string(1, char('0' - i)), util::str(i));
    }
    ASSERT_EQ("-4096", util::str(-4096));
}

TEST(String, FromDouble)
{
    ASSERT_EQ("1", util::str(1.0));
    ASSERT_EQ("1.5", util::str(1.5));
    ASSERT_EQ("2.5", util::str(2.5));
    ASSERT_EQ("-2.5", util::str(-2.5));
    ASSERT_EQ("-1.5", util::str(-1.5));
}

TEST(String, FromBool)
{
    ASSERT_EQ("false", util::str(false));
    ASSERT_EQ("true", util::str(true));
}

TEST(String, FromMPZ)
{
    ASSERT_EQ("0", util::str(mpz_class("0")));
    ASSERT_EQ("123456", util::str(mpz_class("123456")));
    ASSERT_EQ("1234567890123456789012345", util::str(mpz_class("1234567890123456789012345")));
}

TEST(String, FromMPF)
{
    ASSERT_EQ("123.45", util::str(mpf_class(123.45)));
    ASSERT_EQ("12.345", util::str(mpf_class(12.345)));
    ASSERT_EQ("1.2345", util::str(mpf_class(1.2345)));
}
