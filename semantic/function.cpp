#include <output/expr-nodes.h>

#include "function.h"
#include "class.h"
#include "node-base.h"
#include "compiling-space.h"

using namespace semantic;

util::sptr<output::Function const> Function::compile(
        util::sref<SymbolTable> st, bool class_space) const
{
    return util::mkptr(new output::RegularFunction(
            name, param_names, this->_compileBody(st, class_space)));
}

util::sptr<output::Expression const> Function::compileToLambda(
                    util::sref<SymbolTable> st, bool class_space) const
{
    return util::mkptr(new output::Lambda(
            this->pos, this->param_names, this->_compileBody(st, class_space)
          , false));
}

util::sptr<output::Statement const> Function::_compileBody(
        util::sref<SymbolTable> st, bool class_space) const
{
    CompilingSpace body_space(pos, st, param_names, class_space);
    body.compile(body_space);
    return body_space.deliver();
}

util::sptr<output::Function const> RegularAsyncFunction::compile(
        util::sref<SymbolTable> st, bool class_space) const
{
    return util::mkptr(new output::RegularAsyncFunction(
            name, param_names, async_param_index, this->_compileBody(st, class_space)));
}

util::sptr<output::Expression const> RegularAsyncFunction::compileToLambda(
                    util::sref<SymbolTable> st, bool class_space) const
{
    return util::mkptr(new output::RegularAsyncLambda(
                this->pos, this->param_names, this->async_param_index
              , this->_compileBody(st, class_space)));
}

util::sptr<output::Statement const> RegularAsyncFunction::_compileBody(
        util::sref<SymbolTable> st, bool class_space) const
{
    RegularAsyncCompilingSpace body_space(pos, st, param_names, class_space);
    body.compile(body_space);
    return body_space.deliver();
}
