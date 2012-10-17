#include <sstream>
#include <gtest/gtest.h>

#include "../pointer.h"

TEST(Pointer, Id)
{
    ASSERT_EQ("0", util::id(nullptr).str());

    std::stringstream os1;
    util::id id_os(&os1);
    os1 << &os1;
    ASSERT_EQ(os1.str(), id_os.str());
}

TEST(Pointer, SRef)
{
    std::stringstream os1;
    util::sref<std::stringstream> sstream_ref(&os1);
    util::sref<std::ostream> base_ref(sstream_ref);
    base_ref->operator<<(2011) << 0 << 2 << 0 << 3;
    ASSERT_EQ("20110203", os1.str());

    std::stringstream os2;
    sstream_ref = util::mkref(os2);
    base_ref = sstream_ref.convert<std::ostream>();
    base_ref->operator<<(12) << 10;
    ASSERT_EQ("1210", os2.str());

    int x = 1;
    int y = 1;
    int const* ax = &x;
    int const* ay = &y;
    ASSERT_EQ(util::mkref(x).cp(), util::mkref(y).cp());
    ASSERT_NE(util::mkref(ax).cp(), util::mkref(ay).cp());
    y = 2;
    ASSERT_LT(util::mkref(x).cp(), util::mkref(y).cp());
    x = 3;
    ASSERT_LT(util::mkref(y).cp(), util::mkref(x).cp());

    ASSERT_TRUE(util::mkref(x).not_nul());
    ASSERT_TRUE(util::mkref(y).not_nul());
    ASSERT_TRUE(util::mkref(ax).not_nul());
    ASSERT_TRUE(util::mkref(ay).not_nul());
    ASSERT_TRUE(util::sref<int const>(nullptr).nul());

    ASSERT_EQ(3, util::mkref(x).cp());
    ASSERT_EQ(2, util::mkref(y).cp());

    std::stringstream os4;
    util::sref<int const> ref(&x);
    os4 << &x;
    ASSERT_EQ(os4.str(), ref.id().str());

    ASSERT_EQ("0", util::sref<int const>(nullptr).id().str());
}

TEST(Pointer, SPtr)
{
    static int count = 0;

    struct counter {
        counter()
        {
            ++count;
        }

        ~counter()
        {
            --count;
        }
    };

    struct counter_inherit
        : public counter
    {};

    {
        util::sptr<counter> count0(new counter);
        ASSERT_TRUE(count0.not_nul());
        ASSERT_TRUE((*count0).not_nul());
        ASSERT_EQ(1, count);
        util::sref<counter> refcount0 = *count0;
        util::sptr<counter> count1(std::move(count0));
        ASSERT_EQ(1, count);
        util::sref<counter> refcount1 = *count1;
        ASSERT_EQ(refcount0.id().str(), refcount1.id().str());
        ASSERT_EQ(refcount0.id().str(), count1.id().str());
        ASSERT_TRUE(count0.nul());
        ASSERT_TRUE((*count0).nul());

        util::sptr<counter> count2(new counter_inherit);
        count1 = std::move(count2);
        ASSERT_EQ(1, count);
    }
    ASSERT_EQ(0, count);
}
