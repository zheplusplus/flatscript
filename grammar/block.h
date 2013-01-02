#ifndef __STEKIN_GRAMMAR_BLOCK_H__
#define __STEKIN_GRAMMAR_BLOCK_H__

#include <semantic/fwd-decl.h>
#include <util/arrays.h>

#include "fwd-decl.h"

namespace grammar {

    struct Block {
        util::sptr<semantic::Filter> compile(util::sptr<semantic::Filter> filter) const;

        void addStmt(util::sptr<Statement const> stmt);
        void addFunc(util::sptr<Function const> func);

        Block() = default;

        Block(Block const&) = delete;

        Block(Block&& rhs)
            : _stmts(std::move(rhs._stmts))
            , _funcs(std::move(rhs._funcs))
        {}
    private:
        util::ptrarr<Statement const> _stmts;
        util::ptrarr<Function const> _funcs;
    };

}

#endif /* __STEKIN_GRAMMAR_BLOCK_H__ */
