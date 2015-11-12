#include <gtest/gtest.h>

#include <util/arrays.h>

namespace {

    struct Base {
        virtual ~Base() {};

        explicit Base(std::string const& v)
            : value(v)
        {}

        virtual std::string getValue() const
        {
            return "Base::" + value;
        }

        std::string const value;
    };

    struct Derived
        : Base
    {
        explicit Derived(std::string const& v)
            : Base(v)
        {}

        std::string getValue() const
        {
            return "Derived::" + value;
        }
    };

    struct AnotherDerived
        : Base
    {
        AnotherDerived()
            : Base("")
        {}

        std::string getValue() const
        {
            return "Another derived";
        }
    };

}

TEST(Arrays, PushElementOrAnotherArray)
{
    util::ptrarr<Base const> f;
    f.append(util::mkptr(new Base("0a")));
    f.append(util::mkptr(new Base("1b")));

    util::ptrarr<Base const> i;
    i.append(util::mkptr(new Derived("0c")))
     .append(util::mkptr(new Derived("1d")))
     .append(std::move(f))
     ;
    ASSERT_TRUE(f.empty());
    ASSERT_EQ(4, i.size());
    ASSERT_EQ("Derived::0c", i[0]->getValue());
    ASSERT_EQ("Derived::1d", i[1]->getValue());
    ASSERT_EQ("Base::0a", i[2]->getValue());
    ASSERT_EQ("Base::1b", i[3]->getValue());
}

TEST(Arrays, Convert)
{
    util::ptrarr<Derived const> d;
    d.append(util::mkptr(new Derived("Hello")))
     .append(util::mkptr(new Derived("World")))
     ;
    util::ptrarr<Base const> b(std::move(d));
    ASSERT_TRUE(d.empty());
    ASSERT_EQ(2, b.size());
    ASSERT_EQ("Derived::Hello", b[0]->getValue());
    ASSERT_EQ("Derived::World", b[1]->getValue());
}

TEST(Arrays, Map)
{
    util::ptrarr<Base const> i;
    i.append(util::mkptr(new Base("util")))
     .append(util::mkptr(new Derived("arrays")))
     .append(util::mkptr(new AnotherDerived))
     ;
    auto x(i.map([&](util::sptr<Base const> const& obj, int)
                 {
                     return util::mkptr(new std::string(obj->getValue()));
                 }));
    ASSERT_EQ(3, x.size());

    auto y(i.mapv([&](util::sptr<Base const> const& obj, int index)
                  {
                      return index * 1000 + obj->getValue().size();
                  }));
    ASSERT_EQ(3, y.size());
    ASSERT_EQ(10, y[0]);
    ASSERT_EQ(1015, y[1]);
    ASSERT_EQ(2015, y[2]);
}

TEST(Arrays, Any)
{
    util::ptrarr<int const> i;
    ASSERT_FALSE(i.any([&](util::sptr<int const> const&, int)
                       {
                           return true;
                       }));
    i.append(util::mkptr(new int(4)))
     .append(util::mkptr(new int(1)))
     .append(util::mkptr(new int(1)))
     ;
    ASSERT_TRUE(i.any([&](util::sptr<int const> const&, int)
                      {
                          return true;
                      }));
}

TEST(Arrays, Iter)
{
    std::vector<std::string> value_result;
    std::vector<int> index_result;
    util::ptrarr<Base const> i;
    i.append(util::mkptr(new Base("Good")))
     .append(util::mkptr(new Derived("Day")))
     .append(util::mkptr(new AnotherDerived))
     .iter([&](util::sptr<Base const> const& obj, int index)
           {
               value_result.push_back(obj->getValue());
               index_result.push_back(index);
           });
    ASSERT_EQ(3, value_result.size());
    ASSERT_EQ("Base::Good", value_result[0]);
    ASSERT_EQ("Derived::Day", value_result[1]);
    ASSERT_EQ("Another derived", value_result[2]);
    ASSERT_EQ(3, index_result.size());
    ASSERT_EQ(0, index_result[0]);
    ASSERT_EQ(1, index_result[1]);
    ASSERT_EQ(2, index_result[2]);
}

TEST(Arrays, DeliverRange)
{
    util::ptrarr<Base const> i;
    i.append(util::mkptr(new Base("selenite")))
     .append(util::mkptr(new Derived("emerald")))
     .append(util::mkptr(new AnotherDerived))
     ;

    util::ptrarr<Base const> d(i.deliverRange(0, 1));
    ASSERT_EQ(3, i.size());
    ASSERT_TRUE(i[0].nul());
    ASSERT_FALSE(i[1].nul());
    ASSERT_EQ("Derived::emerald", i[1]->getValue());
    ASSERT_FALSE(i[2].nul());
    ASSERT_EQ("Another derived", i[2]->getValue());

    ASSERT_EQ(1, d.size());
    ASSERT_FALSE(d[0].nul());
    ASSERT_EQ("Base::selenite", d[0]->getValue());
}

TEST(KvArrays, Map)
{
    util::ptrkvarr<Base const> i;
    i.append(util::mkptr(new Base("ruby")), util::mkptr(new Base("topaz")))
     .append(util::mkptr(new Derived("flame")), util::mkptr(new AnotherDerived))
     .append(util::mkptr(new AnotherDerived), util::mkptr(new Derived("lightening")))
     ;

    auto x(i.map([&](util::ptrkv<Base const> const& kv, int index)
                 {
                     int key(kv.key->getValue().size() + kv.value->getValue().size());
                     return util::mkkv(util::mkptr(new int(key)), util::mkptr(new int(index)));
                 }));

    auto y(i.mapv([&](util::ptrkv<Base const> const& kv, int)
                  {
                      return kv.key->getValue() + "-" + kv.value->getValue();
                  }));

    ASSERT_EQ(3, i.size());
    ASSERT_EQ("Base::ruby", i[0].key->getValue());
    ASSERT_EQ("Base::topaz", i[0].value->getValue());
    ASSERT_EQ("Derived::flame", i[1].key->getValue());
    ASSERT_EQ("Another derived", i[1].value->getValue());
    ASSERT_EQ("Another derived", i[2].key->getValue());
    ASSERT_EQ("Derived::lightening", i[2].value->getValue());
    ASSERT_EQ(3, x.size());
    ASSERT_EQ(3, y.size());
    ASSERT_EQ("Base::ruby-Base::topaz", y[0]);
    ASSERT_EQ("Derived::flame-Another derived", y[1]);
    ASSERT_EQ("Another derived-Derived::lightening", y[2]);
}
