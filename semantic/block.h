#ifndef __STEKIN_SEMANTIC_BLOCK_H__
#define __STEKIN_SEMANTIC_BLOCK_H__

#include <string>

#include <output/fwd-decl.h>
#include <util/arrays.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"
#include "node-base.h"
#include "function.h"

namespace semantic {

    struct Block {
        Block() = default;

        Block(Block const&) = delete;

        Block(Block&& rhs)
            : _stmts(std::move(rhs._stmts))
            , _funcs(std::move(rhs._funcs))
        {}

        void compile(CompilingSpace& space) const;

        void addStmt(util::sptr<Statement const> stmt);
        void defFunc(misc::position const& pos
                   , std::string const& name
                   , std::vector<std::string> const& param_names
                   , util::sptr<Filter> body);
        void append(Block following);
    private:
        util::ptrarr<Statement const> _stmts;
        util::ptrarr<Function const> _funcs;
    };

}

#endif /* __STEKIN_SEMANTIC_BLOCK_H__ */
