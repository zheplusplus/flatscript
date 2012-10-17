#ifndef __STEKIN_GRAMMAR_CLAUSE_BUILDER_H__
#define __STEKIN_GRAMMAR_CLAUSE_BUILDER_H__

#include <list>
#include <vector>
#include <string>

#include <util/pointer.h>
#include <misc/pos-type.h>

#include "acceptor.h"
#include "fwd-decl.h"

namespace grammar {

    struct AcceptorStack {
        void add(int level, util::sptr<Acceptor> acc);

        void nextStmt(int level, util::sptr<Statement const> stmt);
        void nextFunc(int level, util::sptr<Function const> func);

        void matchElse(int level, misc::position const& pos);

        Block packAll();

        AcceptorStack();
    private:
        void _fillTo(int level, misc::position const& pos);
        void _shrinkTo(int level);
        void _prepareLevel(int level, misc::position const& pos);
    private:
        struct AcceptorOfPack
            : public Acceptor
        {
            AcceptorOfPack()
                : Acceptor(misc::position())
            {}

            void acceptStmt(util::sptr<Statement const> stmt);
            void acceptFunc(util::sptr<Function const> func);

            void deliverTo(util::sref<Acceptor>) {}

            Block pack();
        private:
            Block _pack;
        };

        std::list<util::sptr<Acceptor>> _acceptors;
        util::sref<AcceptorOfPack> const _packer;

        util::sref<AcceptorOfPack> _prepare1stAcceptor();
    };

    struct ClauseBuilder {
        void addArith(int indent_len, util::sptr<Expression const> arith);
        void addVarDef(int indent_len, std::string const& name, util::sptr<Expression const> init);
        void addReturn(int indent_len, util::sptr<Expression const> ret_val);
        void addReturnNothing(int indent_len, misc::position const& pos);

        void addFunction(int indent_len
                       , misc::position const& pos
                       , std::string const& name
                       , std::vector<std::string> const& params);
        void addIf(int indent_len, util::sptr<Expression const> condition);
        void addIfnot(int indent_len, util::sptr<Expression const> condition);
        void addElse(int indent_len, misc::position const& pos);

        util::sptr<flchk::Filter> buildAndClear();
    private:
        AcceptorStack _stack;
    };

}

#endif /* __STEKIN_GRAMMAR_CLAUSE_BUILDER_H__ */
