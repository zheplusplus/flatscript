#ifndef __STEKIN_GRAMMAR_ACCEPTOR_H__
#define __STEKIN_GRAMMAR_ACCEPTOR_H__

#include <flowcheck/fwd-decl.h>
#include <util/pointer.h>
#include <misc/pos-type.h>

#include "node-base.h"
#include "function.h"

namespace grammar {

    struct Acceptor {
        virtual void acceptFunc(util::sptr<Function const> func) = 0;
        virtual void acceptStmt(util::sptr<Statement> stmt) = 0;
        virtual void deliverTo(util::sref<Acceptor> acc) = 0;

        virtual void acceptElse(misc::position const& else_pos);

        virtual ~Acceptor() {}

        misc::position const pos;
    protected:
        explicit Acceptor(misc::position const& ps)
            : pos(ps)
        {}

        Acceptor(Acceptor const&) = delete;
    };

    struct IfAcceptor
        : public Acceptor
    {
        void acceptFunc(util::sptr<Function const> func);
        void acceptStmt(util::sptr<Statement> stmt);
        void deliverTo(util::sref<Acceptor> acc);
        void acceptElse(misc::position const& else_pos);

        IfAcceptor(misc::position const& pos, util::sptr<flchk::Expression const> predicate)
            : Acceptor(pos)
            , _predicate(std::move(predicate))
            , _last_else_pos_or_nul_if_not_matched(nullptr)
            , _current_branch(&_consequence)
        {}
    private:
        bool _elseMatched() const;
    private:
        util::sptr<flchk::Expression const> _predicate;

        util::sptr<misc::position> _last_else_pos_or_nul_if_not_matched;
        Block* _current_branch;

        Block _consequence;
        Block _alternative;
    };

    struct IfnotAcceptor
        : public Acceptor
    {
        void acceptFunc(util::sptr<Function const> func);
        void acceptStmt(util::sptr<Statement> stmt);
        void deliverTo(util::sref<Acceptor> acc);

        IfnotAcceptor(misc::position const& pos, util::sptr<flchk::Expression const> predicate)
            : Acceptor(pos)
            , _predicate(std::move(predicate))
        {}
    private:
        util::sptr<flchk::Expression const> _predicate;

        Block _alternative;
    };

    struct FunctionAcceptor
        : public Acceptor
    {
        void acceptFunc(util::sptr<Function const> func);
        void acceptStmt(util::sptr<Statement> stmt);
        void deliverTo(util::sref<Acceptor> acc);

        FunctionAcceptor(misc::position const& pos
                       , std::string const& func_name
                       , std::vector<std::string> const& params)
            : Acceptor(pos)
            , name(func_name)
            , param_names(params)
        {}

        std::string const name;
        std::vector<std::string> const param_names;
    private:
        Block _body;
    };

}

#endif /* __STEKIN_GRAMMAR_ACCEPTOR_H__ */
