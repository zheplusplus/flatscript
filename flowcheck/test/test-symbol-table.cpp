#include <gtest/gtest.h>

#include <proto/function.h>
#include <test/common.h>
#include <test/phony-errors.h>

#include "test-common.h"
#include "../symbol-table.h"
#include "../function.h"
#include "../func-body-filter.h"
#include "../node-base.h"

using namespace test;

struct SymbolTableTest
    : public FlowcheckTest
{
    SymbolTableTest()
        : block(nullptr)
        , symbols(nullptr)
    {}

    void SetUp()
    {
        FlowcheckTest::SetUp();
        symbols.reset(new flchk::SymbolTable);
        block.reset(new proto::Block);
    }

    util::sref<flchk::SymbolTable const> refSym()
    {
        return *symbols;
    }

    util::sptr<flchk::Filter> mkBody()
    {
        return util::mkptr(new flchk::FuncBodyFilter(
                    misc::position(), refSym(), std::vector<std::string>()));
    }

    util::sptr<proto::Block> block;
    util::sptr<flchk::SymbolTable> symbols;
};

TEST_F(SymbolTableTest, DefVar)
{
    misc::position pos(1);
    symbols->defVar(pos, "nerv");
    symbols->defVar(pos, "seele");
    symbols->defVar(pos, "lilith");
    ASSERT_FALSE(error::hasError());
    flchk::SymbolTable inner_symbols(pos, refSym(), std::vector<std::string>());
    inner_symbols.defVar(pos, "nerv");
    inner_symbols.defVar(pos, "seele");
    inner_symbols.defVar(pos, "adam");
    inner_symbols.defVar(pos, "eve");
    ASSERT_FALSE(error::hasError());
}

TEST_F(SymbolTableTest, RefLocalVar)
{
    misc::position pos(2);
    symbols->defVar(pos, "nerv");
    symbols->defVar(pos, "seele");
    symbols->defVar(pos, "lilith");

    symbols->compileRef(pos, "nerv")->write();
    symbols->compileRef(pos, "seele")->write();
    symbols->compileRef(pos, "lilith")->write();
    ASSERT_FALSE(error::hasError());

    flchk::SymbolTable inner_symbols(pos, refSym(), std::vector<std::string>());
    inner_symbols.defVar(pos, "nerv");
    inner_symbols.defVar(pos, "seele");
    inner_symbols.defVar(pos, "adam");
    inner_symbols.defVar(pos, "eve");

    inner_symbols.compileRef(pos, "nerv")->write();
    inner_symbols.compileRef(pos, "seele")->write();
    inner_symbols.compileRef(pos, "adam")->write();
    inner_symbols.compileRef(pos, "eve")->write();
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

TEST_F(SymbolTableTest, RedefVar)
{
    misc::position pos(5);
    misc::position err_pos0(200);
    misc::position err_pos1(201);

    symbols->defVar(pos, "suzuhara");
    symbols->defVar(pos, "aida");

    symbols->defVar(err_pos0, "suzuhara");
    symbols->defVar(err_pos1, "aida");
    ASSERT_TRUE(error::hasError());
    std::vector<VarRedefRec> redefs = getLocalRedefs();
    ASSERT_EQ(2, redefs.size());
    ASSERT_EQ(pos, redefs[0].prev_pos);
    ASSERT_EQ(err_pos0, redefs[0].this_pos);
    ASSERT_EQ("suzuhara", redefs[0].name);
    ASSERT_EQ(pos, redefs[1].prev_pos);
    ASSERT_EQ(err_pos1, redefs[1].this_pos);
    ASSERT_EQ("aida", redefs[1].name);

    clearErr();
    flchk::SymbolTable inner_symbols(pos, refSym(), std::vector<std::string>());
    inner_symbols.defVar(pos, "aida");
    inner_symbols.defVar(pos, "suzuhara");
    ASSERT_FALSE(error::hasError());
}

TEST_F(SymbolTableTest, VarRefBeforeDef)
{
    misc::position pos(6);
    misc::position ref_pos0(300);
    misc::position ref_pos1(301);
    symbols->defVar(pos, "katsuragi");

    flchk::SymbolTable inner_symbols_a(pos, refSym(), std::vector<std::string>());
    inner_symbols_a.compileRef(ref_pos0, "katsuragi");
    inner_symbols_a.compileRef(ref_pos1, "katsuragi");
    inner_symbols_a.defVar(pos, "katsuragi");
    ASSERT_TRUE(error::hasError());
    std::vector<InvalidRefRec> invalid_refs = getInvalidRefs();
    ASSERT_EQ(1, invalid_refs.size());
    ASSERT_EQ(pos, invalid_refs[0].def_pos);
    ASSERT_EQ(2, invalid_refs[0].ref_positions.size());
        ASSERT_EQ(ref_pos0, invalid_refs[0].ref_positions[0]);
        ASSERT_EQ(ref_pos1, invalid_refs[0].ref_positions[1]);
    ASSERT_EQ("katsuragi", invalid_refs[0].name);

    clearErr();
    symbols->defVar(pos, "penpen");
    symbols->compileRef(pos, "penpen");

    flchk::SymbolTable inner_symbols_b(pos, refSym(), std::vector<std::string>());
    inner_symbols_b.compileRef(pos, "katsuragi");
    inner_symbols_b.defVar(pos, "penpen");
    ASSERT_FALSE(error::hasError());
}
