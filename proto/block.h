#ifndef __STEKIN_PROTO_BLOCK_H__
#define __STEKIN_PROTO_BLOCK_H__

#include <vector>

#include <util/pointer.h>

#include "fwd-decl.h"
#include "node-base.h"

namespace proto {

    struct Block
        : public Statement
    {
        Block(Block const&) = delete;

        Block() = default;

        void addStmt(util::sptr<Statement const> stmt);
        void addFunc(util::sptr<Function const> func);

        void write() const;
    private:
        std::vector<util::sptr<Statement const>> _stmts;
        std::vector<util::sptr<Function const>> _funcs;
    };

}

#endif /* __STEKIN_PROTO_BLOCK_H__ */
