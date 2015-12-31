#ifndef __STEKIN_SEMANTIC_FUNCTION_H__
#define __STEKIN_SEMANTIC_FUNCTION_H__

#include <output/function.h>

#include "node-base.h"

namespace semantic {

    struct Function {
        virtual ~Function() {}
        Function(Function const&) = delete;

        Function(misc::position const& ps
               , std::string const& func_name
               , std::vector<std::string> const& params
               , util::sptr<Statement const> func_body)
            : pos(ps)
            , name(func_name)
            , param_names(params)
            , body(std::move(func_body))
        {}

        util::sptr<output::Function const> compile(util::sref<SymbolTable> st) const
        {
            return this->compile(st, false);
        }

        virtual util::sptr<output::Function const> compile(
                    util::sref<SymbolTable> st, bool class_scope) const;
        virtual util::sptr<output::Expression const> compileToLambda(
                    util::sref<SymbolTable> st, bool class_scope) const;

        misc::position const pos;
        std::string const name;
        std::vector<std::string> const param_names;
        util::sptr<Statement const> const body;
    protected:
        virtual util::sptr<output::Statement const> _compileBody(
                    util::sref<SymbolTable> st, bool class_scope) const;
    };

    struct RegularAsyncFunction
        : Function
    {
        RegularAsyncFunction(misc::position const& pos
                           , std::string const& func_name
                           , std::vector<std::string> const& params
                           , int async_param_idx
                           , util::sptr<Statement const> func_body)
            : Function(pos, func_name, params, std::move(func_body))
            , async_param_index(async_param_idx)
        {}

        int const async_param_index;

        util::sptr<output::Function const> compile(
                    util::sref<SymbolTable> st, bool class_scope) const;
        util::sptr<output::Expression const> compileToLambda(
                    util::sref<SymbolTable> st, bool class_scope) const;
    protected:
        util::sptr<output::Statement const> _compileBody(
                    util::sref<SymbolTable> st, bool class_scope) const;
    };

    struct Lambda
        : Expression
    {
        Lambda(misc::position const& pos
             , std::vector<std::string> const& p
             , util::sptr<Statement const> b)
                : Expression(pos)
                , param_names(p)
                , body(std::move(b))
        {}

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;

        bool isAsync() const { return false; }

        std::vector<std::string> const param_names;
        util::sptr<Statement const> const body;
    };

    struct RegularAsyncLambda
        : Lambda
    {
        RegularAsyncLambda(misc::position const& pos
                         , std::vector<std::string> const& params
                         , int async_param_idx
                         , util::sptr<Statement const> body)
            : Lambda(pos, params, std::move(body))
            , async_param_index(async_param_idx)
        {}

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;

        int const async_param_index;
    };

}

#endif /* __STEKIN_SEMANTIC_FUNCTION_H__ */
