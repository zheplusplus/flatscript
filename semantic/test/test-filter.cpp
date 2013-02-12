#include <gtest/gtest.h>

#include <output/function.h>
#include <test/phony-errors.h>
#include <test/phony-warnings.h>
#include <test/common.h>

#include "test-common.h"
#include "../expr-nodes.h"
#include "../compiling-space.h"
#include "../function.h"

using namespace test;

struct FilterTest
    : SemanticTest
{
    FilterTest()
        : _space(nullptr)
    {}

    void SetUp()
    {
        SemanticTest::SetUp();
        _space = new semantic::CompilingSpace;
    }

    void TearDown()
    {
        delete _space;
        SemanticTest::TearDown();
    }

    util::sref<semantic::SymbolTable> refSym()
    {
        return _space->sym();
    }

    semantic::CompilingSpace* _space;
};

TEST_F(FilterTest, Filter)
{
    misc::position pos(1);

    util::sptr<semantic::BinaryOp> binary(
            new semantic::BinaryOp(pos
                                 , util::mkptr(new semantic::IntLiteral(pos, "1"))
                                 , "+"
                                 , util::mkptr(new semantic::FloatLiteral(pos, "235.7"))));

    semantic::Filter filter0;
    refSym()->defName(pos, "kobayakawa");
    refSym()->defName(pos, "yutaka");
    refSym()->defName(pos, "tamura");
    refSym()->defName(pos, "hiyori");
    refSym()->defName(pos, "minami");

    filter0.addArith(pos, util::mkptr(new semantic::BoolLiteral(pos, true)));
    filter0.defName(pos, "soujirou", std::move(binary));
    filter0.defName(pos, "iwasaki", util::mkptr(new semantic::Reference(pos, "minami")));

    util::sptr<semantic::Filter> filter_consq(new semantic::Filter);
    misc::position pos_consq(100);
    filter_consq->addArith(pos_consq, util::mkptr(
                                        new semantic::Reference(pos_consq, "kobayakawa")));

    util::sptr<semantic::Filter> filter_alter(new semantic::Filter);
    misc::position pos_alter(101);
    filter_alter->addArith(pos_alter , util::mkptr(new semantic::Reference(pos_alter, "yutaka")));
    filter_alter->addArith(pos_alter , util::mkptr(new semantic::Reference(pos_alter, "hiyori")));

    filter0.addBranch(pos
                    , util::mkptr(new semantic::Reference(pos, "tamura"))
                    , std::move(filter_consq)
                    , std::move(filter_alter));
    filter0.addReturnNothing(pos);

    compile(filter0, refSym())->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (FWD_DECL, "iwasaki")
            (ARITHMETICS)
                (pos, BOOLEAN, "true")
            (ARITHMETICS)
                (pos, BINARY_OP, "[=]")
                    (pos, REFERENCE, "iwasaki")
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
            (RETURN)
                (pos, UNDEFINED)
        (SCOPE_END)
    ;
}

TEST_F(FilterTest, TerminatedError)
{
    misc::position pos(2);
    misc::position pos_error(200);
    misc::position pos_ignored(201);

    semantic::Filter filter0;
    filter0.defName(pos, "patricia", util::mkptr(new semantic::Reference(pos, "martin")));
    filter0.addReturnNothing(pos);
    filter0.addReturn(pos_error, util::mkptr(new semantic::Reference(pos, "patty")));
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

    semantic::Filter filter0;

    util::sptr<semantic::Filter> filter_consq(new semantic::Filter);
    filter_consq->addArith(pos, util::mkptr(new semantic::Reference(pos, "ayano")));
    filter_consq->addReturnNothing(pos_warning);

    util::sptr<semantic::Filter> filter_alter(new semantic::Filter);
    filter_alter->addArith(pos, util::mkptr(new semantic::BoolLiteral(pos, true)));

    filter0.addBranch(pos
                    , util::mkptr(new semantic::Reference(pos, "minegishi"))
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

    semantic::Filter filter0;

    util::sptr<semantic::Filter> filter_consq(new semantic::Filter);
    filter_consq->addArith(pos, util::mkptr(new semantic::IntLiteral(pos, "20110411")));

    util::sptr<semantic::Filter> filter_alter(new semantic::Filter);
    filter_alter->addReturn(pos_warning, util::mkptr(new semantic::Reference(pos, "kogami")));

    filter0.addBranch(pos
                    , util::mkptr(new semantic::Reference(pos, "akira"))
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

    semantic::Filter filter0;

    util::sptr<semantic::Filter> filter_consq(new semantic::Filter);
    filter_consq->addReturn(pos_warning_consq, util::mkptr(new semantic::FloatLiteral(pos, ".1")));

    util::sptr<semantic::Filter> filter_alter(new semantic::Filter);
    filter_alter->addReturn(pos_warning_alter, util::mkptr(new semantic::Reference(pos, "minoru")));

    filter0.addBranch(pos
                    , util::mkptr(new semantic::Reference(pos, "shiraishi"))
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
    refSym()->defName(pos, "yui");
    refSym()->defName(pos, "narumi");

    util::sptr<semantic::BinaryOp> binary(
            new semantic::BinaryOp(pos
                                 , util::mkptr(new semantic::IntLiteral(pos, "2048"))
                                 , ">"
                                 , util::mkptr(new semantic::FloatLiteral(pos, "11235.8"))));

    semantic::Filter filter0;

    util::sptr<semantic::Filter> filter_consq(new semantic::Filter);
    misc::position pos_consq(600);
    filter_consq->addArith(pos_consq, util::mkptr(new semantic::Reference(pos_consq, "yui")));

    util::sptr<semantic::Filter> filter_alter(new semantic::Filter);
    misc::position pos_alter(601);
    filter_alter->addArith(pos_alter, util::mkptr(new semantic::Reference(pos_alter, "narumi")));

    filter0.addBranch(pos, std::move(binary), std::move(filter_consq), std::move(filter_alter));
    filter0.addReturnNothing(pos);

    compile(filter0, refSym())->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (SCOPE_BEGIN)
                (ARITHMETICS)
                    (pos_alter, REFERENCE, "narumi")
            (SCOPE_END)
            (RETURN)
                (pos, UNDEFINED)
        (SCOPE_END)
    ;
}

TEST_F(FilterTest, TwoPathBranchFoldedOnTrue)
{
    misc::position pos(7);
    refSym()->defName(pos, "yui");
    refSym()->defName(pos, "narumi");

    util::sptr<semantic::BinaryOp> binary(
            new semantic::BinaryOp(pos
                                 , util::mkptr(new semantic::IntLiteral(pos, "2048"))
                                 , "<"
                                 , util::mkptr(new semantic::FloatLiteral(pos, "11235.8"))));

    semantic::Filter filter0;

    util::sptr<semantic::Filter> filter_consq(new semantic::Filter);
    misc::position pos_consq(700);
    filter_consq->addArith(pos_consq, util::mkptr(new semantic::Reference(pos_consq, "yui")));

    util::sptr<semantic::Filter> filter_alter(new semantic::Filter);
    misc::position pos_alter(701);
    filter_alter->addArith(pos_alter, util::mkptr(new semantic::Reference(pos_alter, "narumi")));

    filter0.addBranch(pos, std::move(binary), std::move(filter_consq), std::move(filter_alter));
    filter0.addReturnNothing(pos);

    compile(filter0, refSym())->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (SCOPE_BEGIN)
            (ARITHMETICS)
                (pos_consq, REFERENCE, "yui")
            (SCOPE_END)
            (RETURN)
                (pos, UNDEFINED)
        (SCOPE_END)
    ;
}

TEST_F(FilterTest, IfNotFoldedOnFalse)
{
    misc::position pos(8);
    refSym()->defName(pos, "narumi");

    util::sptr<semantic::BinaryOp> binary(
            new semantic::BinaryOp(pos
                                 , util::mkptr(new semantic::IntLiteral(pos, "2048"))
                                 , ">"
                                 , util::mkptr(new semantic::FloatLiteral(pos, "11235.8"))));

    semantic::Filter filter0;

    util::sptr<semantic::Filter> filter_alter(new semantic::Filter);
    misc::position pos_alter(801);
    filter_alter->addArith(pos_alter, util::mkptr(new semantic::Reference(pos_alter, "narumi")));

    filter0.addBranchAlterOnly(pos, std::move(binary), std::move(filter_alter));
    filter0.addReturnNothing(pos);

    compile(filter0, refSym())->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (SCOPE_BEGIN)
                (ARITHMETICS)
                    (pos_alter, REFERENCE, "narumi")
            (SCOPE_END)
            (RETURN)
                (pos, UNDEFINED)
        (SCOPE_END)
    ;
}

TEST_F(FilterTest, IfNotFoldedOnTrue)
{
    misc::position pos(8);
    refSym()->defName(pos, "narumi");

    util::sptr<semantic::BinaryOp> binary(
            new semantic::BinaryOp(pos
                                 , util::mkptr(new semantic::IntLiteral(pos, "2048"))
                                 , "<"
                                 , util::mkptr(new semantic::FloatLiteral(pos, "11235.8"))));

    semantic::Filter filter0;

    util::sptr<semantic::Filter> filter_alter(new semantic::Filter);
    misc::position pos_alter(801);
    filter_alter->addArith(pos_alter, util::mkptr(new semantic::Reference(pos_alter, "narumi")));

    filter0.addBranchAlterOnly(pos, std::move(binary), std::move(filter_alter));
    filter0.addReturnNothing(pos);

    compile(filter0, refSym())->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (SCOPE_BEGIN)
            (SCOPE_END)
            (RETURN)
                (pos, UNDEFINED)
        (SCOPE_END)
    ;
}
