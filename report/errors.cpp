#include <iostream>
#include <algorithm>

#include "errors.h"

static bool has_error = false;

extern int yylineno;

bool error::hasError()
{
    return has_error;
}

void yyerror(std::string const& msg)
{
    has_error = true;
    std::cerr << "Line " << yylineno << ":" << std::endl;
    std::cerr << "    " <<  msg << std::endl;
}

void error::tabAsIndent(misc::position const& pos)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    use tab as indent is forbidden." << std::endl;
}

void error::badIndent(misc::position const& pos)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    indent not exactly 4 spaces." << std::endl;
}

void error::invalidChar(misc::position const& pos, int character)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    invalid character " << char(character)
              << " (decimal value: " << character << ")." << std::endl;
}

void error::modifyName(misc::position const& pos, std::string const& name)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    attempt to modify name: " << name << std::endl;
}

void error::sliceStepOmitted(misc::position const& pos)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    slice step omitted, or omit the colon together." << std::endl;
}

void error::elseNotMatchIf(misc::position const& else_pos)
{
    has_error = true;
    std::cerr << else_pos.str() << std::endl;
    std::cerr << "    `else' does not match an `if'." << std::endl;
}

void error::ifAlreadyMatchElse(misc::position const& prev_else_pos
                             , misc::position const& this_else_pos)
{
    has_error = true;
    std::cerr << this_else_pos.str() << std::endl;
    std::cerr << "    another `else' already matches the `if' at " << prev_else_pos.str()
              << std::endl;
}

void error::excessiveIndent(misc::position const& pos)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    excessive indentation" << std::endl;
}

void error::flowTerminated(misc::position const& this_pos, misc::position const& prev_pos)
{
    has_error = true;
    std::cerr << this_pos.str() << std::endl;
    std::cerr << "    flow already terminated at " << prev_pos.str() << std::endl;
}

void error::forbidDefFunc(misc::position const& pos, std::string const& name)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    " << "attempt define Function `" << name << "' but forbidden here."
              << std::endl;
}

void error::forbidDefName(misc::position const& pos, std::string const& name)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    " << "attempt define name `" << name << "' but forbidden here."
              << std::endl;
}

void error::nameAlreadyInLocal(misc::position const& prev_def_pos
                             , misc::position const& this_def_pos
                             , std::string const& name)
{
    has_error = true;
    std::cerr << this_def_pos.str() << std::endl;
    std::cerr << "    name `" << name << "' already defined." << std::endl;
    std::cerr << "    see previous definition in local at " << prev_def_pos.str() << std::endl;
}

void error::nameRefBeforeDef(misc::position const& def_pos
                          , std::list<misc::position> const& ref_positions
                          , std::string const& name)
{
    has_error = true;
    std::cerr << def_pos.str() << std::endl;
    std::cerr << "    name `" << name << "' definition after reference. see references at:"
              << std::endl;
    std::for_each(ref_positions.begin()
                , ref_positions.end()
                , [&](misc::position const& pos)
                  {
                      std::cerr << "    - " << pos.str() << std::endl;
                  });
}

void error::nameNotDef(misc::position const& ref_pos, std::string const& name)
{
    has_error = true;
    std::cerr << ref_pos.str() << std::endl;
    std::cerr << "    name `" << name << "' not defined." << std::endl;
}

void error::binaryOpNotAvai(misc::position const& pos
                          , std::string const& op_img
                          , std::string const& lhst_name
                          , std::string const& rhst_name)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    no available binary operation " << op_img << " for type `" << lhst_name
              << "' and `" << rhst_name << "'." << std::endl;
}

void error::preUnaryOpNotAvai(misc::position const& pos
                            , std::string const& op_img
                            , std::string const& rhst_name)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    no available prefix unary operation " << op_img << " for type `" << rhst_name
              << "'." << std::endl;
}

static std::string instantiate_trace(misc::trace const& trace)
{
    return trace.str("    | instantiate from here: ") + "    +-- instantiation stacktrace";
}

void error::conflictReturnType(std::string const& prev_ret_type_name
                             , std::string const& this_ret_type_name
                             , misc::trace const& trace)
{
    has_error = true;
    std::cerr << "    Function return type conflict" << std::endl;
    std::cerr << "    | - > previous return type: " << prev_ret_type_name << std::endl;
    std::cerr << "    | - - - > this return type: " << this_ret_type_name << std::endl;
    std::cerr << instantiate_trace(trace) << std::endl;
}

void error::returnTypeUnresolvable(std::string const& name, int arg_count, misc::trace const& trace)
{
    has_error = true;
    std::cerr << "Function return type is not resolvable:" << std::endl;
    std::cerr << "    name: `" << name << "' arg_count: " << arg_count << std::endl;
    std::cerr << instantiate_trace(trace) << std::endl;
}

void error::condNotBool(misc::position const& pos, std::string const& actual_type)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    condition type is not boolean, actual type: " << actual_type << std::endl;
}

void error::requestNameNotCallable(misc::position const& call_pos)
{
    has_error = true;
    std::cerr << call_pos.str() << std::endl;
    std::cerr << "    name not callable." << std::endl;
}

void error::callNameArgCountWrong(misc::position const& call_pos, int actual, int wanted)
{
    has_error = true;
    std::cerr << call_pos.str() << std::endl;
    std::cerr << "    call name with " << actual << " arguments, but " << wanted << " needed."
              << std::endl;
}

void error::listMemberTypesNotSame(misc::position const& pos)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    members in the list have different types." << std::endl;
}

void error::memberCallNotFound(misc::position const& pos
                             , std::string const& type_name
                             , std::string const& call_name)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    type `" << type_name << "' does not have member function `" << call_name
              << "'." << std::endl;
}

void error::pipeReferenceNotInListContext(misc::position const& pos)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    pipeline reference not in list context." << std::endl;
}

void error::pipeNotApplyOnList(misc::position const& pos)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    pipeline not applied on a list." << std::endl;
}

void error::featureNotSupportWrapListInClosure(misc::position const& pos)
{
    has_error = true;
    std::cerr << pos.str() << std::endl;
    std::cerr << "    feature not supported: wrap list in closure." << std::endl;
    std::cerr << "    will be fixed in future." << std::endl;
}
