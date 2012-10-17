#include <gtest/gtest.h>

#include "../vector-append.h"

TEST(VectorAppend, PtrsAppend)
{
    util::sptr<int> i(nullptr);
    std::vector<util::sptr<int>> integers;
    integers.push_back(util::mkptr(new int(1)));
    integers.push_back(util::mkptr(new int(2)));
    integers.push_back(util::mkptr(new int(4)));
    std::vector<util::sptr<int>> integers_rv;
    integers_rv.push_back(util::mkptr(new int(3)));
    integers_rv.push_back(util::mkptr(new int(5)));
    integers_rv.push_back(util::mkptr(new int(7)));
    util::ptrs_append(integers, std::move(integers_rv));
    ASSERT_EQ(6, integers.size());
    i.reset(new int(1));
    ASSERT_EQ(*i, *integers[0]);
    i.reset(new int(2));
    ASSERT_EQ(*i, *integers[1]);
    i.reset(new int(4));
    ASSERT_EQ(*i, *integers[2]);
    i.reset(new int(3));
    ASSERT_EQ(*i, *integers[3]);
    i.reset(new int(5));
    ASSERT_EQ(*i, *integers[4]);
    i.reset(new int(7));
    ASSERT_EQ(*i, *integers[5]);

    integers_rv.clear();
    integers_rv.push_back(util::mkptr(new int(-1)));
    integers_rv.push_back(util::mkptr(new int(-2)));
    integers_rv.push_back(util::mkptr(new int(-4)));

    integers = util::ptrs_append(std::move(integers), std::move(integers_rv));
    ASSERT_EQ(9, integers.size());
    i.reset(new int(1));
    ASSERT_EQ(*i, *integers[0]);
    i.reset(new int(2));
    ASSERT_EQ(*i, *integers[1]);
    i.reset(new int(4));
    ASSERT_EQ(*i, *integers[2]);
    i.reset(new int(3));
    ASSERT_EQ(*i, *integers[3]);
    i.reset(new int(5));
    ASSERT_EQ(*i, *integers[4]);
    i.reset(new int(7));
    ASSERT_EQ(*i, *integers[5]);
    i.reset(new int(-1));
    ASSERT_EQ(*i, *integers[6]);
    i.reset(new int(-2));
    ASSERT_EQ(*i, *integers[7]);
    i.reset(new int(-4));
    ASSERT_EQ(*i, *integers[8]);
}

TEST(VectorAppend, VectorExtend)
{
    std::vector<int> former{ 1, 2, 4 };
    std::vector<int> latter{ 3, 5, 7 };
    util::vec_extend(former, latter);
    ASSERT_EQ(6, former.size());
    ASSERT_EQ(1, former[0]);
    ASSERT_EQ(2, former[1]);
    ASSERT_EQ(4, former[2]);
    ASSERT_EQ(3, former[3]);
    ASSERT_EQ(5, former[4]);
    ASSERT_EQ(7, former[5]);
}
