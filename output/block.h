#ifndef __STEKIN_OUTPUT_BLOCK_H__
#define __STEKIN_OUTPUT_BLOCK_H__

#include <set>

#include <util/arrays.h>

#include "node-base.h"
#include "function.h"

namespace output {

    struct Block
        : Statement
    {
        Block() = default;

        void write(std::ostream& os) const;
        int count() const;
        bool mayThrow() const;

        void append(util::sptr<Block> b)
        {
            this->_stmts.append(std::move(b->_stmts));
            this->_funcs.append(std::move(b->_funcs));
            this->_local_decls.insert(b->_local_decls.begin(), b->_local_decls.end());
        }

        void setLocalDecls(std::set<std::string> decls)
        {
            this->_local_decls = std::move(decls);
        }

        void addStmt(util::sptr<Statement const> stmt)
        {
            this->_stmts.append(std::move(stmt));
        }

        void addFunc(util::sptr<Function const> func)
        {
            this->_funcs.append(std::move(func));
        }
    protected:
        util::ptrarr<Statement const> _stmts;
        util::ptrarr<Function const> _funcs;
        std::set<std::string> _local_decls;
    };

}

#endif /* __STEKIN_OUTPUT_BLOCK_H__ */
