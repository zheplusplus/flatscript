#ifndef __STEKIN_OUTPUT_BLOCK_H__
#define __STEKIN_OUTPUT_BLOCK_H__

#include <vector>

#include <util/arrays.h>

#include "node-base.h"
#include "function.h"

namespace output {

    struct Block
        : Statement
    {
        Block() = default;
        Block(Block const&) = delete;

        void write(std::ostream& os) const;
        void addStmt(util::sptr<Statement const> stmt);
        void addFunc(util::sptr<Function const> func);
        void append(util::sptr<Block> b);
    private:
        util::ptrarr<Statement const> _stmts;
        util::ptrarr<Function const> _funcs;
    };

}

#endif /* __STEKIN_OUTPUT_BLOCK_H__ */
