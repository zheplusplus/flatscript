#include <gtest/gtest.h>

#include <grammar/expr-nodes.h>
#include <test/phony-errors.h>

#include "test-common.h"

using namespace test;

typedef GrammarTest RegExpTest;

TEST_F(RegExpTest, Ok)
{
    misc::position pos(1);

    grammar::makeRegEx(pos, "/a/")->reduceAsExpr()->compile(nulScope());
    grammar::makeRegEx(pos, "/ab/")->reduceAsExpr()->compile(nulScope());
    grammar::makeRegEx(pos, "/()/")->reduceAsExpr()->compile(nulScope());
    grammar::makeRegEx(pos, "/[]/")->reduceAsExpr()->compile(nulScope());
    grammar::makeRegEx(pos, "/[]()/")->reduceAsExpr()->compile(nulScope());
    grammar::makeRegEx(pos, "/([]())/")->reduceAsExpr()->compile(nulScope());
    grammar::makeRegEx(pos, "/a+b(a|b)/")->reduceAsExpr()->compile(nulScope());

    grammar::makeRegEx(pos, "/a/g")->reduceAsExpr()->compile(nulScope());
    grammar::makeRegEx(pos, "/ab/i")->reduceAsExpr()->compile(nulScope());
    grammar::makeRegEx(pos, "/()/m")->reduceAsExpr()->compile(nulScope());
    grammar::makeRegEx(pos, "/[]/ig")->reduceAsExpr()->compile(nulScope());
    grammar::makeRegEx(pos, "/[]()/gm")->reduceAsExpr()->compile(nulScope());
    grammar::makeRegEx(pos, "/([]())/im")->reduceAsExpr()->compile(nulScope());
    grammar::makeRegEx(pos, "/a+b(a|b)/mig")->reduceAsExpr()->compile(nulScope());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, REGEXP, "/a/")
        (pos, REGEXP, "/ab/")
        (pos, REGEXP, "/()/")
        (pos, REGEXP, "/[]/")
        (pos, REGEXP, "/[]()/")
        (pos, REGEXP, "/([]())/")
        (pos, REGEXP, "/a+b(a|b)/")

        (pos, REGEXP, "/a/g")
        (pos, REGEXP, "/ab/i")
        (pos, REGEXP, "/()/m")
        (pos, REGEXP, "/[]/ig")
        (pos, REGEXP, "/[]()/gm")
        (pos, REGEXP, "/([]())/im")
        (pos, REGEXP, "/a+b(a|b)/mig")
    ;
}

TEST_F(RegExpTest, BadModifiers)
{
    misc::position pos(2);

    grammar::makeRegEx(pos, "/abc/ii");
    grammar::makeRegEx(pos, "/abc/ic");
    grammar::makeRegEx(pos, "/abc/a");
    grammar::makeRegEx(pos, "/abc/bi");
    ASSERT_TRUE(error::hasError());

    std::vector<InvalidRegExpRec> recs(getInvalidRegExpRecs());
    ASSERT_EQ(4, recs.size());
    ASSERT_EQ(pos, recs[0].pos);
    EXPECT_EQ("duplicated flag i in ii", recs[0].message);
    ASSERT_EQ(pos, recs[1].pos);
    EXPECT_EQ("unknown flag c in ic", recs[1].message);
    ASSERT_EQ(pos, recs[2].pos);
    EXPECT_EQ("unknown flag a in a", recs[2].message);
    ASSERT_EQ(pos, recs[3].pos);
    EXPECT_EQ("unknown flag b in bi", recs[3].message);
}
