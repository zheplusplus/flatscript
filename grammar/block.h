#ifndef __STEKIN_GRAMMAR_BLOCK_H__
#define __STEKIN_GRAMMAR_BLOCK_H__

#include <flowcheck/fwd-decl.h>
#include <util/pointer.h>

#include "fwd-decl.h"

namespace grammar {

    struct Block {
        util::sptr<flchk::Filter> compile(util::sptr<flchk::Filter> filter) const;

        void addStmt(util::sptr<Statement> stmt);
        void addFunc(util::sptr<Function const> func);

        Block() = default;

        Block(Block const&) = delete;

        Block(Block&& rhs)
            : _stmts(std::move(rhs._stmts))
            , _funcs(std::move(rhs._funcs))
        {}
    private:
        std::vector<util::sptr<Statement>> _stmts;
        std::vector<util::sptr<Function const>> _funcs;
    };

}

#endif /* __STEKIN_GRAMMAR_BLOCK_H__ */
