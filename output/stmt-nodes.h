#ifndef __STEKIN_OUTPUT_STATEMENT_NODES_H__
#define __STEKIN_OUTPUT_STATEMENT_NODES_H__

#include <vector>

#include "node-base.h"
#include "methods.h"

namespace output {

    struct Branch
        : Statement
    {
        Branch(util::sptr<Expression const> p
             , util::sptr<Statement const> c
             , util::sptr<Statement const> a)
                : predicate(std::move(p))
                , consequence(std::move(c))
                , alternative(std::move(a))
        {}

        void write(std::ostream& os) const;
        bool mayThrow() const { return predicate->mayThrow()
            || consequence->mayThrow() || alternative->mayThrow(); }

        util::sptr<Expression const> const predicate;
        util::sptr<Statement const> const consequence;
        util::sptr<Statement const> const alternative;
    };

    struct AsyncCallResultDef
        : Statement
    {
        AsyncCallResultDef(util::sptr<Expression const> ar, bool nd)
            : async_result(std::move(ar))
            , need_decl(nd)
        {}

        void write(std::ostream& os) const;
        bool mayThrow() const { return async_result->mayThrow(); }

        util::sptr<Expression const> const async_result;
        bool const need_decl;
    };

    struct Export
        : Statement
    {
        Export(std::vector<std::string> e, util::sptr<Expression const> v);

        void write(std::ostream& os) const;
        int count() const;
        bool mayThrow() const { return value->mayThrow(); }

        std::vector<std::string> const export_point;
        util::sptr<Expression const> const value;
    };

    struct ThisDeclaration
        : Statement
    {
        void write(std::ostream& os) const;
        bool mayThrow() const { return false; }
    };

    struct ExceptionStall
        : Statement
    {
        ExceptionStall(util::sptr<Statement const> t, util::sptr<Statement const> c)
            : try_block(std::move(t))
            , catch_block(std::move(c))
        {}

        void write(std::ostream& os) const;
        int count() const;
        bool mayThrow() const { return catch_block->mayThrow(); }

        util::sptr<Statement const> const try_block;
        util::sptr<Statement const> const catch_block;
    };

    struct ExprScheme
        : Statement
    {
        ExprScheme(Method m, util::sptr<Expression const> e)
            : method(std::move(m))
            , expr(std::move(e))
        {}

        void write(std::ostream& os) const;
        bool mayThrow() const { return method->mayThrow(expr); }

        Method const method;
        util::sptr<Expression const> const expr;
    };

}

#endif /* __STEKIN_OUTPUT_STATEMENT_NODES_H__ */
