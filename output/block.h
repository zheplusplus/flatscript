#ifndef __STEKIN_OUTPUT_BLOCK_H__
#define __STEKIN_OUTPUT_BLOCK_H__

#include <set>

#include <util/arrays.h>

#include "node-base.h"

namespace output {

    struct Block
        : Statement
    {
        Block() = default;
        Block(Block const&) = delete;

        void write(std::ostream& os) const;
        int count() const;
        void addStmt(util::sptr<Statement const> stmt);
        void addFunc(util::sptr<Function const> func);
        void append(util::sptr<Block> b);
        void setLocalDecls(std::set<std::string> const& decls);
    private:
        util::ptrarr<Statement const> _stmts;
        util::ptrarr<Function const> _funcs;
        std::set<std::string> _local_decls;
    };

}

#endif /* __STEKIN_OUTPUT_BLOCK_H__ */
