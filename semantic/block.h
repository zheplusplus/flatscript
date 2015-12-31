#ifndef __STEKIN_SEMANTIC_BLOCK_H__
#define __STEKIN_SEMANTIC_BLOCK_H__

#include <util/arrays.h>

#include "function.h"

namespace semantic {

    struct Block
        : Statement
    {
        explicit Block(misc::position const& pos)
            : Statement(pos)
        {}

        bool isAsync() const;
        void checkNoAsync(misc::position const& check_pos) const;
        void append(Block following);
        void compile(util::sref<Scope> scope) const;

        void addStmt(util::sptr<Statement const> stmt)
        {
            this->_stmts.append(std::move(stmt));
        }

        void addFunc(util::sptr<Function const> func)
        {
            this->_funcs.append(std::move(func));
        }

        util::ptrarr<Function const> const& getFuncs() const
        {
            return this->_funcs;
        }
    private:
        util::ptrarr<Statement const> _stmts;
        util::ptrarr<Function const> _funcs;
    };

}

#endif /* __STEKIN_SEMANTIC_BLOCK_H__ */
