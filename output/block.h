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

        void write(std::ostream& os) const;
        int count() const;
        void append(util::sptr<Block> b);

        void setLocalDecls(std::set<std::string> decls)
        {
            this->_local_decls = std::move(decls);
        }

        void addClass(util::sptr<Class const> cls)
        {
            this->_classes.append(std::move(cls));
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
        util::ptrarr<Class const> _classes;
        util::ptrarr<Statement const> _stmts;
        util::ptrarr<Function const> _funcs;
        std::set<std::string> _local_decls;
    };

}

#endif /* __STEKIN_OUTPUT_BLOCK_H__ */
