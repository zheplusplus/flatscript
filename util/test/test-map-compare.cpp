#include <gtest/gtest.h>

#include "../map-compare.h"

static bool one_less(std::map<int, long long> const& lhs, std::map<int, long long> const& rhs)
{
    if (util::map_less(lhs, rhs)) {
        return !util::map_less(rhs, lhs);
    }
    return util::map_less(rhs, lhs);
}

static bool eq(std::map<int, long long> const& lhs, std::map<int, long long> const& rhs)
{
    return (!util::map_less(lhs, rhs)) && (!util::map_less(rhs, lhs));
}

TEST(MapCompare, MapCompare)
{
    std::map<int, long long> a;
    std::map<int, long long> b;
    ASSERT_TRUE(eq(a, b));

    a.insert(std::pair<int, long long>(10, 100));
    ASSERT_TRUE(one_less(a, b));

    b.insert(std::pair<int, long long>(10, 123));
    ASSERT_TRUE(one_less(a, b));

    b[10] = 100;
    ASSERT_TRUE(eq(a, b));

    b[10] = 99;
    ASSERT_TRUE(one_less(a, b));

    a[10] = 99;
    ASSERT_TRUE(eq(a, b));

    a.insert(std::pair<int, long long>(20, 100));
    b.insert(std::pair<int, long long>(30, 100));
    ASSERT_TRUE(one_less(a, b));

    b.insert(std::pair<int, long long>(20, 100));
    a.insert(std::pair<int, long long>(30, 100));
    ASSERT_TRUE(eq(a, b));
}