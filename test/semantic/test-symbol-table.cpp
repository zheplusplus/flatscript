#include <gtest/gtest.h>

#include <semantic/symbol-table.h>
#include <semantic/stmt-nodes.h>
#include <semantic/expr-nodes.h>
#include <semantic/scope-impl.h>
#include <test/common.h>
#include <test/phony-errors.h>

#include "test-common.h"

using namespace test;

struct SymbolTableTest
    : SemanticTest
{
    SymbolTableTest()
        : _scope(nullptr)
    {}

    void SetUp()
    {
        SemanticTest::SetUp();
        this->_scope = semantic::Scope::global();
    }

    util::sref<semantic::SymbolTable> refSym()
    {
        return this->_scope->sym();
    }

    util::sref<semantic::Scope> scope()
    {
        return *this->_scope;
    }

    util::sptr<semantic::Scope> _scope;
};

TEST_F(SymbolTableTest, DefName)
{
    misc::position pos(1);
    refSym()->defName(pos, "nerv");
    refSym()->defName(pos, "seele");
    refSym()->defName(pos, "lilith");
    ASSERT_FALSE(error::hasError());
    semantic::SyncFunctionScope inner_scope(pos, refSym(), std::vector<std::string>(), false);
    inner_scope.sym()->defName(pos, "nerv");
    inner_scope.sym()->defName(pos, "seele");
    inner_scope.sym()->defName(pos, "adam");
    inner_scope.sym()->defName(pos, "eve");
    ASSERT_FALSE(error::hasError());
}

TEST_F(SymbolTableTest, RefLocalName)
{
    misc::position pos(2);
    refSym()->defName(pos, "nerv");
    refSym()->defName(pos, "seele");
    refSym()->defName(pos, "lilith");

    refSym()->compileRef(pos, "nerv")->str();
    refSym()->compileRef(pos, "seele")->str();
    refSym()->compileRef(pos, "lilith")->str();
    ASSERT_FALSE(error::hasError());

    semantic::SyncFunctionScope inner_scope(pos, refSym(), std::vector<std::string>(), false);
    inner_scope.sym()->defName(pos, "nerv");
    inner_scope.sym()->defName(pos, "seele");
    inner_scope.sym()->defName(pos, "adam");
    inner_scope.sym()->defName(pos, "eve");

    inner_scope.sym()->compileRef(pos, "nerv")->str();
    inner_scope.sym()->compileRef(pos, "seele")->str();
    inner_scope.sym()->compileRef(pos, "adam")->str();
    inner_scope.sym()->compileRef(pos, "eve")->str();
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, REFERENCE, "nerv")
        (pos, REFERENCE, "seele")
        (pos, REFERENCE, "lilith")

        (pos, REFERENCE, "nerv")
        (pos, REFERENCE, "seele")
        (pos, REFERENCE, "adam")
        (pos, REFERENCE, "eve")
    ;
}

TEST_F(SymbolTableTest, RedefName)
{
    misc::position pos(5);
    misc::position err_pos0(500);
    misc::position err_pos1(501);

    refSym()->defName(pos, "suzuhara");
    refSym()->defName(pos, "aida");

    refSym()->defName(err_pos0, "suzuhara");
    refSym()->defName(err_pos1, "aida");
    ASSERT_TRUE(error::hasError());
    std::vector<NameAlreadyInLocalRec> redefs = getNameAlreadyInLocalRecs();
    ASSERT_EQ(2, redefs.size());
    ASSERT_EQ(pos, redefs[0].prev_def_pos);
    ASSERT_EQ(err_pos0, redefs[0].this_def_pos);
    ASSERT_EQ("suzuhara", redefs[0].name);
    ASSERT_EQ(pos, redefs[1].prev_def_pos);
    ASSERT_EQ(err_pos1, redefs[1].this_def_pos);
    ASSERT_EQ("aida", redefs[1].name);

    clearErr();
    semantic::SyncFunctionScope inner_scope(pos, refSym(), std::vector<std::string>(), false);
    inner_scope.sym()->defName(pos, "aida");
    inner_scope.sym()->defName(pos, "suzuhara");
    ASSERT_FALSE(error::hasError());
}

TEST_F(SymbolTableTest, NameRefBeforeDef)
{
    misc::position pos(6);
    misc::position ref_pos0(600);
    misc::position ref_pos1(601);
    refSym()->defName(pos, "katsuragi");

    semantic::SyncFunctionScope inner_scope_a(pos, refSym(), std::vector<std::string>(), false);
    inner_scope_a.sym()->compileRef(ref_pos0, "katsuragi");
    inner_scope_a.sym()->compileRef(ref_pos1, "katsuragi");
    inner_scope_a.sym()->defName(pos, "katsuragi");
    ASSERT_TRUE(error::hasError());
    std::vector<NameRefBeforeDefRec> invalid_refs = getNameRefBeforeDefRecs();
    ASSERT_EQ(1, invalid_refs.size());
    ASSERT_EQ(pos, invalid_refs[0].def_pos);
    ASSERT_EQ(2, invalid_refs[0].ref_positions.size());
        ASSERT_EQ(ref_pos0, invalid_refs[0].ref_positions[0]);
        ASSERT_EQ(ref_pos1, invalid_refs[0].ref_positions[1]);
    ASSERT_EQ("katsuragi", invalid_refs[0].name);

    clearErr();
    refSym()->defName(pos, "penpen");
    refSym()->compileRef(pos, "penpen");

    semantic::SyncFunctionScope inner_scope_b(pos, refSym(), std::vector<std::string>(), false);
    inner_scope_b.sym()->compileRef(pos, "katsuragi");
    inner_scope_b.sym()->defName(pos, "penpen");
    ASSERT_FALSE(error::hasError());
}

TEST_F(SymbolTableTest, ExternalAlreadyDef)
{
    misc::position pos(7);
    misc::position ref_pos(700);

    refSym()->defName(pos, "akari");
    semantic::Extern external(ref_pos, std::vector<std::string>({ "akari", "akaza" }));
    external.compile(scope());

    ASSERT_TRUE(error::hasError());
    std::vector<NameAlreadyInLocalRec> redefs(getNameAlreadyInLocalRecs());
    ASSERT_EQ(1, redefs.size());
    ASSERT_EQ(pos, redefs[0].prev_def_pos);
    ASSERT_EQ(ref_pos, redefs[0].this_def_pos);
    ASSERT_EQ("akari", redefs[0].name);
}

TEST_F(SymbolTableTest, ExternalBeforeDef)
{
    misc::position pos(8);
    misc::position ref_pos(800);

    semantic::Extern external(ref_pos, std::vector<std::string>({ "yuru", "yuri" }));
    external.compile(scope());
    refSym()->defName(pos, "yuri");

    ASSERT_TRUE(error::hasError());
    std::vector<NameAlreadyInLocalRec> redefs(getNameAlreadyInLocalRecs());
    ASSERT_EQ(1, redefs.size());
    ASSERT_EQ(ref_pos, redefs[0].prev_def_pos);
    ASSERT_EQ(pos, redefs[0].this_def_pos);
    ASSERT_EQ("yuri", redefs[0].name);
}

TEST_F(SymbolTableTest, ExternalAfterRef)
{
    misc::position pos(9);
    misc::position ref_pos(900);

    refSym()->compileRef(ref_pos, "akane");
    semantic::Extern external(pos, std::vector<std::string>({ "akane" }));
    external.compile(scope());

    ASSERT_TRUE(error::hasError());
    std::vector<NameRefBeforeDefRec> invalid_refs(getNameRefBeforeDefRecs());
    ASSERT_EQ(1, invalid_refs.size());
    ASSERT_EQ(pos, invalid_refs[0].def_pos);
    ASSERT_EQ(1, invalid_refs[0].ref_positions.size());
        ASSERT_EQ(ref_pos, invalid_refs[0].ref_positions[0]);
    ASSERT_EQ("akane", invalid_refs[0].name);
}

TEST_F(SymbolTableTest, CompileRef)
{
    misc::position pos(10);
    misc::position ref_pos(1000);

    refSym()->addExternNames(pos, {"akemi"});
    util::sptr<semantic::Expression const> i(new semantic::IntLiteral(pos, 20121115));
    refSym()->defConst(pos, "kaname", *i);
    refSym()->defName(pos, "miki");

    refSym()->compileRef(ref_pos, "akemi")->str();
    refSym()->compileRef(ref_pos, "kaname")->str();
    refSym()->compileRef(ref_pos, "miki")->str();

    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (ref_pos, IMPORTED_NAME, "akemi")
        (pos, INTEGER, "20121115")
        (ref_pos, REFERENCE, "miki")
    ;
}

TEST_F(SymbolTableTest, ForbidDef)
{
    misc::position pos(11);
    misc::position def_pos(1100);

    semantic::Block block;
    refSym()->defName(pos, "ryou");
    semantic::Block consq_block;
    consq_block.addFunc(util::mkptr(new semantic::Function(
        def_pos, "leela", std::vector<std::string>(), semantic::Block())));
    block.addStmt(util::mkptr(new semantic::Branch(
                     pos
                   , util::mkptr(new semantic::Reference(pos, "ryou"))
                   , std::move(consq_block)
                   , semantic::Block())));

    compile(block, refSym());

    ASSERT_TRUE(error::hasError());

    std::vector<ForbidDefFuncRec> forbidDefs = getForbidDefFuncRecs();
    ASSERT_EQ(1, forbidDefs.size());
    ASSERT_EQ(def_pos, forbidDefs[0].pos);
    ASSERT_EQ("leela", forbidDefs[0].name);
}
