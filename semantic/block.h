#ifndef __STEKIN_SEMANTIC_BLOCK_H__
#define __STEKIN_SEMANTIC_BLOCK_H__

#include <string>

#include <output/fwd-decl.h>
#include <util/arrays.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"
#include "node-base.h"

namespace semantic {

    struct Block {
        Block() = default;

        Block(Block const&) = delete;

        Block(Block&& rhs)
            : _stmts(std::move(rhs._stmts))
            , _funcs(std::move(rhs._funcs))
        {}

        util::sptr<output::Statement const> compile(BaseCompilingSpace&& space) const;
        bool isAsync() const;

        void addStmt(util::sptr<Statement const> stmt);
        void addFunc(util::sptr<Function const> func);
        void append(Block following);
    private:
        util::ptrarr<Statement const> _stmts;
        util::ptrarr<Function const> _funcs;
    };

}

#endif /* __STEKIN_SEMANTIC_BLOCK_H__ */
