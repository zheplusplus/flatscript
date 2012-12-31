#include <sstream>

#include <test/common.h>
#include <test/phony-errors.h>
#include <test/phony-warnings.h>
#include <output/function.h>
#include <util/string.h>

#include "test-common.h"

using namespace test;

std::ostream& test::dummyos()
{
    static std::ostringstream os;
    return os;
}

DataTree& DataTree::operator()(misc::position const& pos
                             , NodeType const& type
                             , std::string const& str)
{
    BaseType::operator()(type, SemanticData(pos), str);
    return *this;
}

DataTree& DataTree::operator()(misc::position const& pos, NodeType const& type)
{
    BaseType::operator()(type, SemanticData(pos));
    return *this;
}

DataTree& DataTree::operator()(misc::position const& pos, NodeType const& type, int size)
{
    BaseType::operator()(type, SemanticData(pos, size));
    return *this;
}

DataTree& DataTree::operator()(misc::position const& pos
                             , NodeType const& type
                             , std::string const& str
                             , int size)
{
    BaseType::operator()(type, SemanticData(pos, size), str);
    return *this;
}

DataTree& DataTree::operator()(NodeType const& type)
{
    BaseType::operator()(type, SemanticData());
    return *this;
}

DataTree& DataTree::operator()(NodeType const& type, std::string const& str)
{
    BaseType::operator()(type, SemanticData(), str);
    return *this;
}

std::ostream& operator<<(std::ostream& os, SemanticData const& data)
{
    return -1 == data.size
        ? (os << data.pos)
        : (os << data.pos << " size=" << data.size);
}

NodeType const test::BOOLEAN("boolean");
NodeType const test::INTEGER("integer");
NodeType const test::FLOATING("floating");
NodeType const test::STRING("string");
NodeType const test::LIST("list");
NodeType const test::BINARY_OP("binary operation");
NodeType const test::PRE_UNARY_OP("prefix unary operation");
NodeType const test::CALL("call");
NodeType const test::REFERENCE("reference");
NodeType const test::IMPORTED_NAME("imported name");
NodeType const test::PIPE_ELEMENT("list element");
NodeType const test::PIPE_INDEX("list index");
NodeType const test::PIPE_KEY("list key");

NodeType const test::LIST_PIPELINE("list pipeline");
NodeType const test::PIPE_MAP("pipe map");
NodeType const test::PIPE_FILTER("pipe filter");

NodeType const test::LIST_SLICE("list slice");
NodeType const test::LIST_SLICE_DEFAULT("list slice default");

NodeType const test::DICT_BEGIN("dictionary begin");
NodeType const test::DICT_END("dictionary end");
NodeType const test::DICT_ITEM("dictionary item");

NodeType const test::NAME_DEF("name definition");

NodeType const test::STATEMENT("statement");
NodeType const test::ARITHMETICS("arithmetics");
NodeType const test::RETURN("return");
NodeType const test::RETURN_NOTHING("return nothing");
NodeType const test::EXPORT("export");
NodeType const test::EXPORT_VALUE("exported value");
NodeType const test::ATTR_SET("attribute set");

NodeType const test::FUNC_DECL("func decl");
NodeType const test::PARAMETER("parameter");

NodeType const test::SCOPE_BEGIN("scope begin");
NodeType const test::SCOPE_END("scope end");

NodeType const test::BRANCH("branch");

void SemanticTest::SetUp()
{
    clearErr();
    clearWarn();
}

void SemanticTest::TearDown()
{
    DataTree::verify();
}
