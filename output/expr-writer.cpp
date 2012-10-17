#include <iostream>
#include <map>

#include <util/str-comprehension.h>

#include "expr-writer.h"
#include "name-mangler.h"

void output::writeInt(mpz_class const& i)
{
    std::cout << i;
}

void output::writeFloat(mpf_class const& d)
{
    std::cout << d;
}

void output::writeBool(bool b)
{
    std::cout << b;
}

void output::writeString(char const* value, int length)
{
    std::cout << util::cstr_repr(value, length);
}

void output::reference(std::string const& name)
{
    std::cout << formVarName(true, name);
}

void output::referenceFunc(std::string const& name, int param_count)
{
    std::cout << formFuncName(name, param_count);
}

namespace {

    std::map<std::string, std::string> genOpMap()
    {
        std::map<std::string, std::string> m;
        m.insert(std::make_pair("=", "=="));
        return m;
    }

    std::map<std::string, std::string> const OP_MAP(genOpMap());

}

void output::writeOperator(std::string const& op_img)
{
    auto r = OP_MAP.find(op_img);
    if (OP_MAP.end() == r) {
        std::cout << " " << op_img << " ";
    } else {
        std::cout << " " << r->second << " ";
    }
}

void output::emptyList()
{
    std::cout << "[]";
}

void output::listBegin()
{
    std::cout << "[";
}

void output::listNextMember()
{
    std::cout << ",";
}

void output::listEnd()
{
    std::cout << "]";
}

void output::callBegin()
{
    std::cout << "(";
}

void output::callEnd()
{
    std::cout << ")";
}

void output::listAppendBegin()
{
    std::cout << ".concat(";
}

void output::listAppendEnd()
{
    std::cout << ")";
}

void output::beginExpr()
{
    std::cout << "(";
}

void output::endExpr()
{
    std::cout << ")";
}
