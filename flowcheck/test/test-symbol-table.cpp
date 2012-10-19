#include <gtest/gtest.h>

#include <proto/function.h>
#include <test/common.h>
#include <test/phony-errors.h>

#include "test-common.h"
#include "../symbol-table.h"
#include "../function.h"
#include "../func-body-filter.h"
#include "../stmt-nodes.h"

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

    util::sref<flchk::SymbolTable> refSym()
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

TEST_F(SymbolTableTest, DefName)
{
    misc::position pos(1);
    symbols->defName(pos, "nerv");
    symbols->defName(pos, "seele");
    symbols->defName(pos, "lilith");
    ASSERT_FALSE(error::hasError());
    flchk::SymbolTable inner_symbols(pos, refSym(), std::vector<std::string>());
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

    symbols->compileRef(pos, "nerv")->stringify(false);
    symbols->compileRef(pos, "seele")->stringify(false);
    symbols->compileRef(pos, "lilith")->stringify(false);
    ASSERT_FALSE(error::hasError());

    flchk::SymbolTable inner_symbols(pos, refSym(), std::vector<std::string>());
    inner_symbols.defName(pos, "nerv");
    inner_symbols.defName(pos, "seele");
    inner_symbols.defName(pos, "adam");
    inner_symbols.defName(pos, "eve");

    inner_symbols.compileRef(pos, "nerv")->stringify(false);
    inner_symbols.compileRef(pos, "seele")->stringify(false);
    inner_symbols.compileRef(pos, "adam")->stringify(false);
    inner_symbols.compileRef(pos, "eve")->stringify(false);
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
    misc::position err_pos0(200);
    misc::position err_pos1(201);

    symbols->defName(pos, "suzuhara");
    symbols->defName(pos, "aida");

    symbols->defName(err_pos0, "suzuhara");
    symbols->defName(err_pos1, "aida");
    ASSERT_TRUE(error::hasError());
    std::vector<NameRedefRec> redefs = getLocalRedefs();
    ASSERT_EQ(2, redefs.size());
    ASSERT_EQ(pos, redefs[0].prev_pos);
    ASSERT_EQ(err_pos0, redefs[0].this_pos);
    ASSERT_EQ("suzuhara", redefs[0].name);
    ASSERT_EQ(pos, redefs[1].prev_pos);
    ASSERT_EQ(err_pos1, redefs[1].this_pos);
    ASSERT_EQ("aida", redefs[1].name);

    clearErr();
    flchk::SymbolTable inner_symbols(pos, refSym(), std::vector<std::string>());
    inner_symbols.defName(pos, "aida");
    inner_symbols.defName(pos, "suzuhara");
    ASSERT_FALSE(error::hasError());
}

TEST_F(SymbolTableTest, NameRefBeforeDef)
{
    misc::position pos(6);
    misc::position ref_pos0(300);
    misc::position ref_pos1(301);
    symbols->defName(pos, "katsuragi");

    flchk::SymbolTable inner_symbols_a(pos, refSym(), std::vector<std::string>());
    inner_symbols_a.compileRef(ref_pos0, "katsuragi");
    inner_symbols_a.compileRef(ref_pos1, "katsuragi");
    inner_symbols_a.defName(pos, "katsuragi");
    ASSERT_TRUE(error::hasError());
    std::vector<InvalidRefRec> invalid_refs = getInvalidRefs();
    ASSERT_EQ(1, invalid_refs.size());
    ASSERT_EQ(pos, invalid_refs[0].def_pos);
    ASSERT_EQ(2, invalid_refs[0].ref_positions.size());
        ASSERT_EQ(ref_pos0, invalid_refs[0].ref_positions[0]);
        ASSERT_EQ(ref_pos1, invalid_refs[0].ref_positions[1]);
    ASSERT_EQ("katsuragi", invalid_refs[0].name);

    clearErr();
    symbols->defName(pos, "penpen");
    symbols->compileRef(pos, "penpen");

    flchk::SymbolTable inner_symbols_b(pos, refSym(), std::vector<std::string>());
    inner_symbols_b.compileRef(pos, "katsuragi");
    inner_symbols_b.defName(pos, "penpen");
    ASSERT_FALSE(error::hasError());
}

TEST_F(SymbolTableTest, ImportAlreadyDef)
{
    misc::position pos(7);
    misc::position ref_pos(400);

    symbols->defName(pos, "akari");
    flchk::Import import(ref_pos, std::vector<std::string>({ "akari", "akaza" }));
    import.compile(refSym());

    ASSERT_TRUE(error::hasError());
    std::vector<NameRedefRec> redefs = getLocalRedefs();
    ASSERT_EQ(1, redefs.size());
    ASSERT_EQ(pos, redefs[0].prev_pos);
    ASSERT_EQ(ref_pos, redefs[0].this_pos);
    ASSERT_EQ("akari", redefs[0].name);
}

TEST_F(SymbolTableTest, ImportBeforeDef)
{
    misc::position pos(8);
    misc::position ref_pos(500);

    flchk::Import import(ref_pos, std::vector<std::string>({ "yuru", "yuri" }));
    import.compile(refSym());
    symbols->defName(pos, "yuri");

    ASSERT_TRUE(error::hasError());
    std::vector<NameRedefRec> redefs = getLocalRedefs();
    ASSERT_EQ(1, redefs.size());
    ASSERT_EQ(ref_pos, redefs[0].prev_pos);
    ASSERT_EQ(pos, redefs[0].this_pos);
    ASSERT_EQ("yuri", redefs[0].name);
}

TEST_F(SymbolTableTest, ImportBeforeRef)
{
    misc::position pos(9);
    misc::position ref_pos(600);

    symbols->compileRef(ref_pos, "akane");
    flchk::Import import(pos, std::vector<std::string>({ "akane" }));
    import.compile(refSym());

    ASSERT_TRUE(error::hasError());
    std::vector<InvalidRefRec> invalid_refs = getInvalidRefs();
    ASSERT_EQ(1, invalid_refs.size());
    ASSERT_EQ(pos, invalid_refs[0].def_pos);
    ASSERT_EQ(1, invalid_refs[0].ref_positions.size());
        ASSERT_EQ(ref_pos, invalid_refs[0].ref_positions[0]);
    ASSERT_EQ("akane", invalid_refs[0].name);
}
