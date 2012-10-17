#include <iostream>

#include "built-in-writer.h"

void output::beginWriterStmt()
{
    std::cout << "console.log(";
}

void output::endWriterStmt()
{
    std::cout << ")";
}
