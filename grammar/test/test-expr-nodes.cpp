#include <gtest/gtest.h>

#include <flowcheck/node-base.h>
#include <flowcheck/filter.h>
#include <flowcheck/function.h>
#include <proto/node-base.h>

#include "test-common.h"
#include "../expr-nodes.h"

using namespace test;

typedef GrammarTest ExprNodesTest;

TEST_F(ExprNodesTest, Literal)
{
    misc::position pos(1);
    std::vector<util::sptr<grammar::Expression const>> list_members;
    list_members.push_back(util::mkptr(new grammar::IntLiteral(pos, "20110813")));
    list_members.push_back(util::mkptr(new grammar::BoolLiteral(pos, false)));

    list_members.push_back(util::mkptr(new grammar::ListLiteral(pos, std::move(list_members))));

    list_members.push_back(util::mkptr(new grammar::StringLiteral(pos, "aki no sora")));
    list_members.push_back(util::mkptr(new grammar::FloatLiteral(pos, "21.02")));
    grammar::ListLiteral ls(pos, std::move(list_members));

    ls.compile()->compile(nulSymbols());

    DataTree::expectOne()
        (pos, LIST, 3)
            (pos, LIST, 2)
                (pos, INTEGER, "20110813")
                (pos, BOOLEAN, "false")
            (pos, STRING, "aki no sora")
            (pos, FLOATING, "21.02")
    ;
}

TEST_F(ExprNodesTest, ListAppending)
{
    misc::position pos(2);
    grammar::ListAppend(pos
                      , util::mkptr(new grammar::Reference(pos, "haruka"))
                      , util::mkptr(new grammar::Reference(pos, "kana")))
        .compile()->compile(nulSymbols());

    DataTree::expectOne()
        (pos, BINARY_OP, "++")
            (pos, REFERENCE, "haruka")
            (pos, REFERENCE, "kana")
    ;
}
