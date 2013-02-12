#include <sstream>

#include <semantic/filter.h>
#include <semantic/node-base.h>
#include <semantic/function.h>
#include <test/common.h>
#include <test/phony-errors.h>

#include "test-common.h"

using namespace test;

std::map<std::string, grammar::TokenType> const test::IMAGE_TYPE_MAP({
    { "(", grammar::OPEN_PAREN },
    { "[", grammar::OPEN_BRACKET },
    { "{", grammar::OPEN_BRACE },
    { ")", grammar::CLOSE_PAREN },
    { "]", grammar::CLOSE_BRACKET },
    { "}", grammar::CLOSE_BRACE },
});

util::sptr<semantic::Filter> test::mkfilter()
{
    return std::move(util::mkptr(new semantic::Filter));
}

util::sref<semantic::SymbolTable> test::nulSymbols()
{
    return util::sref<semantic::SymbolTable>(nullptr);
}

semantic::CompilingSpace& test::nulSpace()
{
    static semantic::CompilingSpace* const n(nullptr);
    return *n;
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

std::string GrammarData::str() const
{
    std::ostringstream os;
    -1 == int_val ? (os << pos) : (os << pos << " int value=" << int_val);
    return os.str();
}

NodeType const test::BOOLEAN("boolean");
NodeType const test::INTEGER("integer");
NodeType const test::FLOATING("floating");
NodeType const test::STRING("string");
NodeType const test::IDENTIFIER("identifier");
NodeType const test::THIS("this");
NodeType const test::LIST_BEGIN("list begin");
NodeType const test::LIST_END("list end");

NodeType const test::UNDEFINED("undefined");
NodeType const test::PRE_UNARY_OP("prefix unary operation");
NodeType const test::BINARY_OP("binary operation");
NodeType const test::CONDITIONAL("conditional expression");
NodeType const test::OPERAND("operand");
NodeType const test::PIPE_ELEMENT("pipe element");
NodeType const test::PIPE_INDEX("pipe index");
NodeType const test::PIPE_KEY("pipe key");
NodeType const test::PIPE_RESULT("pipe result");

NodeType const test::CALL_BEGIN("call begin");
NodeType const test::CALL_END("call end");
NodeType const test::ARGUMENTS("arguments");
NodeType const test::ASYNC_PLACEHOLDER_BEGIN("asynchronous placeholder begin");
NodeType const test::ASYNC_PLACEHOLDER_END("asynchronous placeholder end");
NodeType const test::ASYNC_CALL("asynchronous call");

NodeType const test::LIST_SLICE_BEGIN("list slice begin");
NodeType const test::LIST_SLICE_END("list slice end");

NodeType const test::DICT_BEGIN("dictionary begin");
NodeType const test::DICT_END("dictionary end");
NodeType const test::DICT_ITEM("dictionary item");

NodeType const test::NAME_DEF("name def");
NodeType const test::ARITHMETICS("arithmetics");
NodeType const test::RETURN("return");
NodeType const test::RETURN_NOTHING("return nothing");
NodeType const test::IMPORT("import");
NodeType const test::EXPORT("export");
NodeType const test::EXPORT_VALUE("exported value");
NodeType const test::ATTR_SET("attribute set");

NodeType const test::REGULAR_ASYNC_PARAM_INDEX("regular asynchronous parameter index");
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
