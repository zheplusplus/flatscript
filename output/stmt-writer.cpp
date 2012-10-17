#include <iostream>

#include "stmt-writer.h"
#include "name-mangler.h"

void output::kwReturn()
{
    std::cout << "return ";
}

void output::returnNothing()
{
    kwReturn();
    endOfStatement();
}

void output::kwDeclare(std::string const& name)
{
    std::cout << "var " << formVarName(true, name) << " = ";
}

void output::branchIf()
{
    std::cout << "if ";
}

void output::branchElse()
{
    std::cout << " else ";
}

void output::blockBegin()
{
    std::cout << "{" << std::endl;
}

void output::blockEnd()
{
    std::cout << "}" << std::endl;
}

void output::endOfStatement()
{
    std::cout << ";" << std::endl;
}
