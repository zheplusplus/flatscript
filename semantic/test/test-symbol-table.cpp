#include <gtest/gtest.h>

#include <output/function.h>
#include <test/common.h>
#include <test/phony-errors.h>

#include "test-common.h"
#include "../symbol-table.h"
#include "../function.h"
#include "../func-body-filter.h"
#include "../stmt-nodes.h"
#include "../expr-nodes.h"

using namespace test;

struct SymbolTableTest
    : SemanticTest
{
    SymbolTableTest()
        : symbols(nullptr)
    {}

    void SetUp()
    {
        SemanticTest::SetUp();
        symbols.reset(new semantic::SymbolTable);
    }

    util::sref<semantic::SymbolTable> refSym()
    {
        return *symbols;
    }

    util::sptr<semantic::SymbolTable> symbols;
};

TEST_F(SymbolTableTest, DefName)
{
    misc::position pos(1);
    symbols->defName(pos, "nerv");
    symbols->defName(pos, "seele");
    symbols->defName(pos, "lilith");
    ASSERT_FALSE(error::hasError());
    semantic::SymbolTable inner_symbols(pos, refSym(), std::vector<std::string>());
    inner_symbols.defName(pos, "nerv");
    inner_symbols.defName(pos, "seele");
    inner_symbols.defName(pos, "adam");
    inner_symbols.defName(pos, "eve");
    ASSERT_FALSE(error::hasError());
}

TEST_F(SymbolTableTest, RefLocalName)
{
    misc::position pos(2);
    symbols->defName(pos, "nerv");
    symbols->defName(pos, "seele");
    symbols->defName(pos, "lilith");

    symbols->compileRef(pos, "nerv")->str(false);
    symbols->compileRef(pos, "seele")->str(false);
    symbols->compileRef(pos, "lilith")->str(false);
    ASSERT_FALSE(error::hasError());

    semantic::SymbolTable inner_symbols(pos, refSym(), std::vector<std::string>());
    inner_symbols.defName(pos, "nerv");
    inner_symbols.defName(pos, "seele");
    inner_symbols.defName(pos, "adam");
    inner_symbols.defName(pos, "eve");

    inner_symbols.compileRef(pos, "nerv")->str(false);
    inner_symbols.compileRef(pos, "seele")->str(false);
    inner_symbols.compileRef(pos, "adam")->str(false);
    inner_symbols.compileRef(pos, "eve")->str(false);
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

    symbols->defName(pos, "suzuhara");
    symbols->defName(pos, "aida");

    symbols->defName(err_pos0, "suzuhara");
    symbols->defName(err_pos1, "aida");
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
    semantic::SymbolTable inner_symbols(pos, refSym(), std::vector<std::string>());
    inner_symbols.defName(pos, "aida");
    inner_symbols.defName(pos, "suzuhara");
    ASSERT_FALSE(error::hasError());
}

TEST_F(SymbolTableTest, NameRefBeforeDef)
{
    misc::position pos(6);
    misc::position ref_pos0(600);
    misc::position ref_pos1(601);
    symbols->defName(pos, "katsuragi");

    semantic::SymbolTable inner_symbols_a(pos, refSym(), std::vector<std::string>());
    inner_symbols_a.compileRef(ref_pos0, "katsuragi");
    inner_symbols_a.compileRef(ref_pos1, "katsuragi");
    inner_symbols_a.defName(pos, "katsuragi");
    ASSERT_TRUE(error::hasError());
    std::vector<NameRefBeforeDefRec> invalid_refs = getNameRefBeforeDefRecs();
    ASSERT_EQ(1, invalid_refs.size());
    ASSERT_EQ(pos, invalid_refs[0].def_pos);
    ASSERT_EQ(2, invalid_refs[0].ref_positions.size());
        ASSERT_EQ(ref_pos0, invalid_refs[0].ref_positions[0]);
        ASSERT_EQ(ref_pos1, invalid_refs[0].ref_positions[1]);
    ASSERT_EQ("katsuragi", invalid_refs[0].name);

    clearErr();
    symbols->defName(pos, "penpen");
    symbols->compileRef(pos, "penpen");

    semantic::SymbolTable inner_symbols_b(pos, refSym(), std::vector<std::string>());
    inner_symbols_b.compileRef(pos, "katsuragi");
    inner_symbols_b.defName(pos, "penpen");
    ASSERT_FALSE(error::hasError());
}

TEST_F(SymbolTableTest, ImportAlreadyDef)
{
    misc::position pos(7);
    misc::position ref_pos(700);

    symbols->defName(pos, "akari");
    semantic::Import import(ref_pos, std::vector<std::string>({ "akari", "akaza" }));
    util::sptr<output::Block> block(new output::Block);
    import.compile(refSym(), *block);

    ASSERT_TRUE(error::hasError());
    std::vector<NameAlreadyInLocalRec> redefs = getNameAlreadyInLocalRecs();
    ASSERT_EQ(1, redefs.size());
    ASSERT_EQ(pos, redefs[0].prev_def_pos);
    ASSERT_EQ(ref_pos, redefs[0].this_def_pos);
    ASSERT_EQ("akari", redefs[0].name);
}

TEST_F(SymbolTableTest, ImportBeforeDef)
{
    misc::position pos(8);
    misc::position ref_pos(800);

    semantic::Import import(ref_pos, std::vector<std::string>({ "yuru", "yuri" }));
    util::sptr<output::Block> block(new output::Block);
    import.compile(refSym(), *block);
    symbols->defName(pos, "yuri");

    ASSERT_TRUE(error::hasError());
    std::vector<NameAlreadyInLocalRec> redefs = getNameAlreadyInLocalRecs();
    ASSERT_EQ(1, redefs.size());
    ASSERT_EQ(ref_pos, redefs[0].prev_def_pos);
    ASSERT_EQ(pos, redefs[0].this_def_pos);
    ASSERT_EQ("yuri", redefs[0].name);
}

TEST_F(SymbolTableTest, ImportBeforeRef)
{
    misc::position pos(9);
    misc::position ref_pos(900);

    symbols->compileRef(ref_pos, "akane");
    semantic::Import import(pos, std::vector<std::string>({ "akane" }));
    util::sptr<output::Block> block(new output::Block);
    import.compile(refSym(), *block);

    ASSERT_TRUE(error::hasError());
    std::vector<NameRefBeforeDefRec> invalid_refs = getNameRefBeforeDefRecs();
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

    symbols->imported(pos, "akemi");
    util::sptr<semantic::Expression const> i(new semantic::IntLiteral(pos, 20121115));
    symbols->defConst(pos, "kaname", *i);
    symbols->defName(pos, "miki");

    symbols->compileRef(ref_pos, "akemi")->str(false);
    symbols->compileRef(ref_pos, "kaname")->str(false);
    symbols->compileRef(ref_pos, "miki")->str(false);

    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (ref_pos, IMPORTED_NAME, "akemi")
        (pos, INTEGER, "20121115")
        (ref_pos, REFERENCE, "miki")
    ;
}
