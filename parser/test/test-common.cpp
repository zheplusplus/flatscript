#include <test/common.h>

#include "test-common.h"

using namespace test;

DataTree& DataTree::operator()(misc::position const& pos
                             , NodeType const& type
                             , std::string const& str)
{
    BaseType::operator()(type, ParserData(pos), str);
    return *this;
}

DataTree& DataTree::operator()(misc::position const& pos
                             , int indent
                             , NodeType const& type
                             , std::string const& str)
{
    BaseType::operator()(type, ParserData(indent, pos), str);
    return *this;
}

DataTree& DataTree::operator()(misc::position const& pos, NodeType const& type)
{
    BaseType::operator()(type, ParserData(pos));
    return *this;
}

DataTree& DataTree::operator()(misc::position const& pos, int indent, NodeType const& type)
{
    BaseType::operator()(type, ParserData(indent, pos));
    return *this;
}

std::ostream& operator<<(std::ostream& os, ParserData const& data)
{
    return (-1 == data.indent_level ? os << "no indent "
                                    : os << "indent level: " << data.indent_level << " ")
                        << data.pos;
}

NodeType const test::BOOLEAN("boolean");
NodeType const test::INTEGER("integer");
NodeType const test::FLOATING("floating");
NodeType const test::STRING("string");
NodeType const test::IDENTIFIER("identifier");
NodeType const test::LIST_BEGIN("list begin");
NodeType const test::LIST_END("list end");
NodeType const test::LIST_ELEMENT("list element");
NodeType const test::LIST_INDEX("list index");

NodeType const test::LIST_PIPELINE("list pipeline");
NodeType const test::PIPE_BEGIN("pipe map");
NodeType const test::PIPE_END("pipe filter");
NodeType const test::PIPE_MAP("pipe map");
NodeType const test::PIPE_FILTER("pipe filter");

NodeType const test::BINARY_OP_BEGIN("binary operation begin");
NodeType const test::BINARY_OP_END("binary operation end");
NodeType const test::PRE_UNARY_OP_BEGIN("prefix unary operation begin");
NodeType const test::PRE_UNARY_OP_END("prefix unary operation end");
NodeType const test::OPERAND("operand");

NodeType const test::FUNC_CALL_BEGIN("func call begin");
NodeType const test::FUNC_CALL_END("func call end");
NodeType const test::ARGUMENT("argument");

NodeType const test::VAR_DEF("var def");
NodeType const test::ARITHMETICS("arithmetics");
NodeType const test::RETURN("return");

NodeType const test::FUNC_DEF_HEAD_BEGIN("func def head begin");
NodeType const test::FUNC_DEF_HEAD_END("func def head end");

NodeType const test::CONDITION_BEGIN("condition begin");
NodeType const test::CONDITION_END("condition end");

NodeType const test::BRANCH_IF("branch if");
NodeType const test::BRANCH_IFNOT("branch ifnot");
NodeType const test::BRANCH_ELSE("branch else");
