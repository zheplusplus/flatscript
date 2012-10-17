#ifndef __STEKIN_PROTO_STATEMENT_NODES_H__
#define __STEKIN_PROTO_STATEMENT_NODES_H__

#include "node-base.h"

namespace proto {

    struct Branch
        : public Statement
    {
        Branch(util::sptr<Expression const> p
             , util::sptr<Statement const> consequence_stmt
             , util::sptr<Statement const> alternative_stmt)
                : predicate(std::move(p))
                , consequence(std::move(consequence_stmt))
                , alternative(std::move(alternative_stmt))
        {}

        void write() const;
    public:
        util::sptr<Expression const> const predicate;
        util::sptr<Statement const> const consequence;
        util::sptr<Statement const> const alternative;
    };

    struct Arithmetics
        : public Statement
    {
        explicit Arithmetics(util::sptr<Expression const> e)
            : expr(std::move(e))
        {}

        util::sptr<Expression const> const expr;

        void write() const;
    };

    struct VarDef
        : public Statement
    {
        VarDef(std::string const& n, util::sptr<Expression const> i)
            : name(n)
            , init(std::move(i))
        {}

        std::string const name;
        util::sptr<Expression const> const init;

        void write() const;
    };

    struct Return
        : public Statement
    {
        explicit Return(util::sptr<Expression const> r)
            : ret_val(std::move(r))
        {}

        util::sptr<Expression const> const ret_val;

        void write() const;
    };

    struct ReturnNothing
        : public Statement
    {
        ReturnNothing() = default;

        void write() const;
    };

}

#endif /* __STEKIN_PROTO_STATEMENT_NODES_H__ */
