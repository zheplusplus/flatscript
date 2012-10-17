#ifndef __STEKIN_GRAMMAR_BLOCK_H__
#define __STEKIN_GRAMMAR_BLOCK_H__

#include <list>

#include <flowcheck/fwd-decl.h>
#include <util/pointer.h>

#include "fwd-decl.h"

namespace grammar {

    struct Block {
        util::sptr<flchk::Filter> compile(util::sptr<flchk::Filter> filter) const;

        void addStmt(util::sptr<Statement const> stmt);
        void addFunc(util::sptr<Function const> func);

        Block() = default;

        Block(Block const&) = delete;

        Block(Block&& rhs)
            : _stmts(std::move(rhs._stmts))
            , _funcs(std::move(rhs._funcs))
        {}
    private:
        std::list<util::sptr<Statement const>> _stmts;
        std::list<util::sptr<Function const>> _funcs;
    };

}

#endif /* __STEKIN_GRAMMAR_BLOCK_H__ */
