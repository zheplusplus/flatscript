#include <vector>
#include <gtest/gtest.h>

#include <proto/function.h>
#include <test/phony-errors.h>
#include <test/phony-warnings.h>
#include <test/common.h>

#include "test-common.h"
#include "../expr-nodes.h"
#include "../global-filter.h"
#include "../symbol-def-filter.h"
#include "../function.h"

using namespace test;

typedef FlowcheckTest FilterTest;

TEST_F(FilterTest, GlobalFilter)
{
    misc::position pos(1);

    util::sptr<flchk::BinaryOp> binary(
            new flchk::BinaryOp(pos
                              , util::mkptr(new flchk::IntLiteral(pos, "1"))
                              , "+"
                              , util::mkptr(new flchk::FloatLiteral(pos, "235.7"))));

    flchk::GlobalFilter filter0;
    filter0.getSymbols()->defName(pos, "kobayakawa");
    filter0.getSymbols()->defName(pos, "yutaka");
    filter0.getSymbols()->defName(pos, "tamura");
    filter0.getSymbols()->defName(pos, "hiyori");
    filter0.getSymbols()->defName(pos, "minami");

    filter0.addArith(pos, util::mkptr(new flchk::BoolLiteral(pos, true)));
    filter0.defName(pos, "soujirou", std::move(binary));
    filter0.defName(pos, "iwasaki", util::mkptr(new flchk::Reference(pos, "minami")));

    util::sptr<flchk::Filter> filter_consq(new flchk::SymbolDefFilter(filter0.getSymbols()));
    misc::position pos_consq(100);
    filter_consq->addArith(pos_consq, util::mkptr(new flchk::Reference(pos_consq, "kobayakawa")));

    util::sptr<flchk::Filter> filter_alter(new flchk::SymbolDefFilter(filter0.getSymbols()));
    misc::position pos_alter(101);
    filter_alter->addArith(pos_alter , util::mkptr(new flchk::Reference(pos_alter, "yutaka")));
    filter_alter->addArith(pos_alter , util::mkptr(new flchk::Reference(pos_alter, "hiyori")));

    filter0.addBranch(pos
                    , util::mkptr(new flchk::Reference(pos, "tamura"))
                    , std::move(filter_consq)
                    , std::move(filter_alter));
    filter0.addReturnNothing(pos);

    filter0.compile()->write();

    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ARITHMETICS)
                (pos, BOOLEAN, "true")
            (NAME_DEF, "soujirou")
                (pos, FLOATING, "236.7")
            (NAME_DEF, "iwasaki")
                (pos, REFERENCE, "minami")
            (BRANCH)
            (pos, REFERENCE, "tamura")
            (SCOPE_BEGIN)
                (ARITHMETICS)
                    (pos_consq, REFERENCE, "kobayakawa")
            (SCOPE_END)
            (SCOPE_BEGIN)
                (ARITHMETICS)
                    (pos_alter, REFERENCE, "yutaka")
                (ARITHMETICS)
                    (pos_alter, REFERENCE, "hiyori")
            (SCOPE_END)
            (RETURN_NOTHING)
        (SCOPE_END)
    ;
}

TEST_F(FilterTest, TerminatedError)
{
    misc::position pos(2);
    misc::position pos_error(200);
    misc::position pos_ignored(201);

    flchk::GlobalFilter filter0;
    filter0.defName(pos, "patricia", util::mkptr(new flchk::Reference(pos, "martin")));
    filter0.addReturnNothing(pos);
    filter0.addReturn(pos_error, util::mkptr(new flchk::Reference(pos, "patty")));
    filter0.addReturnNothing(pos_ignored);

    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getFlowTerminatedRecs().size());
    ASSERT_EQ(pos_error, getFlowTerminatedRecs()[0].this_pos);
    ASSERT_EQ(pos, getFlowTerminatedRecs()[0].prev_pos);
}

TEST_F(FilterTest, TerminatedWarningIfConsequence)
{
    misc::position pos(3);
    misc::position pos_warning(300);

    flchk::GlobalFilter filter0;

    util::sptr<flchk::Filter> filter_consq(new flchk::SymbolDefFilter(filter0.getSymbols()));
    filter_consq->addArith(pos, util::mkptr(new flchk::Reference(pos, "ayano")));
    filter_consq->addReturnNothing(pos_warning);

    util::sptr<flchk::Filter> filter_alter(new flchk::SymbolDefFilter(filter0.getSymbols()));
    filter_alter->addArith(pos, util::mkptr(new flchk::BoolLiteral(pos, true)));

    filter0.addBranch(pos
                    , util::mkptr(new flchk::Reference(pos, "minegishi"))
                    , std::move(filter_consq)
                    , std::move(filter_alter));

    EXPECT_FALSE(error::hasError());
    ASSERT_EQ(1, getConsqBranchTerminated().size());
    ASSERT_EQ(pos_warning, getConsqBranchTerminated()[0].pos);
}

TEST_F(FilterTest, TerminatedWarningIfAlternative)
{
    misc::position pos(4);
    misc::position pos_warning(400);

    flchk::GlobalFilter filter0;

    util::sptr<flchk::Filter> filter_consq(new flchk::SymbolDefFilter(filter0.getSymbols()));
    filter_consq->addArith(pos, util::mkptr(new flchk::IntLiteral(pos, "20110411")));

    util::sptr<flchk::Filter> filter_alter(new flchk::SymbolDefFilter(filter0.getSymbols()));
    filter_alter->addReturn(pos_warning, util::mkptr(new flchk::Reference(pos, "kogami")));

    filter0.addBranch(pos
                    , util::mkptr(new flchk::Reference(pos, "akira"))
                    , std::move(filter_consq)
                    , std::move(filter_alter));

    EXPECT_FALSE(error::hasError());
    ASSERT_EQ(1, getAlterBranchTerminated().size());
    ASSERT_EQ(pos_warning, getAlterBranchTerminated()[0].pos);
}

TEST_F(FilterTest, TerminatedWarningBothBranches)
{
    misc::position pos(5);
    misc::position pos_warning_consq(500);
    misc::position pos_warning_alter(501);

    flchk::GlobalFilter filter0;

    util::sptr<flchk::Filter> filter_consq(new flchk::SymbolDefFilter(filter0.getSymbols()));
    filter_consq->addReturn(pos_warning_consq, util::mkptr(new flchk::FloatLiteral(pos, ".1")));

    util::sptr<flchk::Filter> filter_alter(new flchk::SymbolDefFilter(filter0.getSymbols()));
    filter_alter->addReturn(pos_warning_alter, util::mkptr(new flchk::Reference(pos, "minoru")));

    filter0.addBranch(pos
                    , util::mkptr(new flchk::Reference(pos, "shiraishi"))
                    , std::move(filter_consq)
                    , std::move(filter_alter));

    EXPECT_FALSE(error::hasError());
    ASSERT_EQ(1, getBothBranchesTerminated().size());
    ASSERT_EQ(pos_warning_consq, getBothBranchesTerminated()[0].consq_pos);
    ASSERT_EQ(pos_warning_alter, getBothBranchesTerminated()[0].alter_pos);
}

TEST_F(FilterTest, TwoPathBranchFoldedOnFalse)
{
    misc::position pos(6);

    util::sptr<flchk::BinaryOp> binary(
            new flchk::BinaryOp(pos
                              , util::mkptr(new flchk::IntLiteral(pos, "2048"))
                              , ">"
                              , util::mkptr(new flchk::FloatLiteral(pos, "11235.8"))));

    flchk::GlobalFilter filter0;
    filter0.getSymbols()->defName(pos, "yui");
    filter0.getSymbols()->defName(pos, "narumi");

    util::sptr<flchk::Filter> filter_consq(new flchk::SymbolDefFilter(filter0.getSymbols()));
    misc::position pos_consq(600);
    filter_consq->addArith(pos_consq, util::mkptr(new flchk::Reference(pos_consq, "yui")));

    util::sptr<flchk::Filter> filter_alter(new flchk::SymbolDefFilter(filter0.getSymbols()));
    misc::position pos_alter(601);
    filter_alter->addArith(pos_alter, util::mkptr(new flchk::Reference(pos_alter, "narumi")));

    filter0.addBranch(pos, std::move(binary), std::move(filter_consq), std::move(filter_alter));
    filter0.addReturnNothing(pos);

    filter0.compile()->write();

    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
        (ARITHMETICS)
            (pos_alter, REFERENCE, "narumi")
        (RETURN_NOTHING)
        (SCOPE_END)
    ;
}

TEST_F(FilterTest, TwoPathBranchFoldedOnTrue)
{
    misc::position pos(7);

    util::sptr<flchk::BinaryOp> binary(
            new flchk::BinaryOp(pos
                              , util::mkptr(new flchk::IntLiteral(pos, "2048"))
                              , "<"
                              , util::mkptr(new flchk::FloatLiteral(pos, "11235.8"))));

    flchk::GlobalFilter filter0;
    filter0.getSymbols()->defName(pos, "yui");
    filter0.getSymbols()->defName(pos, "narumi");

    util::sptr<flchk::Filter> filter_consq(new flchk::SymbolDefFilter(filter0.getSymbols()));
    misc::position pos_consq(700);
    filter_consq->addArith(pos_consq, util::mkptr(new flchk::Reference(pos_consq, "yui")));

    util::sptr<flchk::Filter> filter_alter(new flchk::SymbolDefFilter(filter0.getSymbols()));
    misc::position pos_alter(701);
    filter_alter->addArith(pos_alter, util::mkptr(new flchk::Reference(pos_alter, "narumi")));

    filter0.addBranch(pos, std::move(binary), std::move(filter_consq), std::move(filter_alter));
    filter0.addReturnNothing(pos);

    filter0.compile()->write();

    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
        (ARITHMETICS)
            (pos_consq, REFERENCE, "yui")
        (RETURN_NOTHING)
        (SCOPE_END)
    ;
}

TEST_F(FilterTest, IfNotFoldedOnFalse)
{
    misc::position pos(8);

    util::sptr<flchk::BinaryOp> binary(
            new flchk::BinaryOp(pos
                              , util::mkptr(new flchk::IntLiteral(pos, "2048"))
                              , ">"
                              , util::mkptr(new flchk::FloatLiteral(pos, "11235.8"))));

    flchk::GlobalFilter filter0;
    filter0.getSymbols()->defName(pos, "narumi");

    util::sptr<flchk::Filter> filter_alter(new flchk::SymbolDefFilter(filter0.getSymbols()));
    misc::position pos_alter(801);
    filter_alter->addArith(pos_alter, util::mkptr(new flchk::Reference(pos_alter, "narumi")));

    filter0.addBranchAlterOnly(pos, std::move(binary), std::move(filter_alter));
    filter0.addReturnNothing(pos);

    filter0.compile()->write();

    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
        (ARITHMETICS)
            (pos_alter, REFERENCE, "narumi")
        (RETURN_NOTHING)
        (SCOPE_END)
    ;
}

TEST_F(FilterTest, IfNotFoldedOnTrue)
{
    misc::position pos(8);

    util::sptr<flchk::BinaryOp> binary(
            new flchk::BinaryOp(pos
                              , util::mkptr(new flchk::IntLiteral(pos, "2048"))
                              , "<"
                              , util::mkptr(new flchk::FloatLiteral(pos, "11235.8"))));

    flchk::GlobalFilter filter0;

    util::sptr<flchk::Filter> filter_alter(new flchk::SymbolDefFilter(filter0.getSymbols()));
    misc::position pos_alter(801);
    filter_alter->addArith(pos_alter, util::mkptr(new flchk::Reference(pos_alter, "narumi")));

    filter0.addBranchAlterOnly(pos, std::move(binary), std::move(filter_alter));
    filter0.addReturnNothing(pos);

    filter0.compile()->write();

    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
        (RETURN_NOTHING)
        (SCOPE_END)
    ;
}
