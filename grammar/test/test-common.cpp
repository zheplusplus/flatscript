#include <flowcheck/global-filter.h>
#include <flowcheck/node-base.h>
#include <flowcheck/function.h>
#include <test/common.h>
#include <test/phony-errors.h>

#include "test-common.h"

using namespace test;

util::sptr<flchk::Filter> test::mkfilter()
{
    return std::move(util::mkptr(new flchk::GlobalFilter));
}

util::sref<flchk::SymbolTable> test::nulSymbols()
{
    return util::sref<flchk::SymbolTable>(nullptr);
}

DataTree& DataTree::operator()(misc::position const& pos
                             , NodeType const& type
                             , std::string const& str)
{
    BaseType::operator()(type, GrammarData(pos), str);
    return *this;
}

DataTree& DataTree::operator()(misc::position const& pos, NodeType const& type)
{
    BaseType::operator()(type, GrammarData(pos));
    return *this;
}

DataTree& DataTree::operator()(misc::position const& pos
                             , NodeType const& type
                             , std::string const& str
                             , int func_arg_size)
{
    BaseType::operator()(type, GrammarData(pos, func_arg_size), str);
    return *this;
}

DataTree& DataTree::operator()(NodeType const& type)
{
    BaseType::operator()(type, GrammarData());
    return *this;
}

DataTree& DataTree::operator()(misc::position const& pos, NodeType const& type, int list_size)
{
    BaseType::operator()(type, GrammarData(pos, list_size));
    return *this;
}

std::ostream& operator<<(std::ostream& os, GrammarData const& data)
{
    return -1 == data.func_arg_size ? (os << data.pos)
                                    : (os << data.pos << " arg size=" << data.func_arg_size);
}

std::string const test::NAME_DEF_FILTERED(" name def filtered");
std::string const test::FUNC_DEF_FILTERED(" func def filtered");

NodeType const test::BOOLEAN("boolean");
NodeType const test::INTEGER("integer");
NodeType const test::FLOATING("floating");
NodeType const test::STRING("string");
NodeType const test::IDENTIFIER("identifier");
NodeType const test::LIST_BEGIN("list begin");
NodeType const test::LIST_END("list end");

NodeType const test::BINARY_OP("binary operation");
NodeType const test::PRE_UNARY_OP("prefix unary operation");
NodeType const test::OPERAND("operand");
NodeType const test::LIST_PIPELINE_BEGIN("list pipeline begin");
NodeType const test::LIST_PIPELINE_END("list pipeline end");
NodeType const test::PIPE_MAP("pipeline map");
NodeType const test::PIPE_FILTER("pipeline filter");
NodeType const test::LIST_ELEMENT("list element");
NodeType const test::LIST_INDEX("list index");

NodeType const test::CALL_BEGIN("call begin");
NodeType const test::CALL_END("call end");
NodeType const test::ARGUMENTS("arguments");
NodeType const test::LIST_SLICE_BEGIN("list slice begin");
NodeType const test::LIST_SLICE_END("list slice end");
NodeType const test::LIST_SLICE_DEFAULT("list slice default");

NodeType const test::DICT_BEGIN("dictionary begin");
NodeType const test::DICT_END("dictionary end");
NodeType const test::DICT_ITEM("dictionary item");

NodeType const test::NAME_DEF("name def");
NodeType const test::ARITHMETICS("arithmetics");
NodeType const test::RETURN("return");
NodeType const test::RETURN_NOTHING("return nothing");
NodeType const test::IMPORT("import");
NodeType const test::ATTR_SET("attribute set");

NodeType const test::FUNC_DEF("func def");
NodeType const test::PARAMETER("parameter");

NodeType const test::BRANCH("branch");
NodeType const test::BRANCH_CONSQ_ONLY("branch consequence only");
NodeType const test::BRANCH_ALTER_ONLY("branch alternative only");
NodeType const test::CONSEQUENCE("consequence");
NodeType const test::ALTERNATIVE("alternative");

NodeType const test::BLOCK_BEGIN("block begin");
NodeType const test::BLOCK_END("block end");

void GrammarTest::SetUp()
{
    clearErr();
}

void GrammarTest::TearDown()
{
    DataTree::verify();
}
