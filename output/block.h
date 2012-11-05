#ifndef __STEKIN_OUTPUT_BLOCK_H__
#define __STEKIN_OUTPUT_BLOCK_H__

#include <vector>

#include <util/pointer.h>

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
    private:
        std::vector<util::sptr<Statement const>> _stmts;
        std::vector<util::sptr<Function const>> _funcs;
    };

}

#endif /* __STEKIN_OUTPUT_BLOCK_H__ */
