#ifndef __STEKIN_GRAMMAR_BLOCK_H__
#define __STEKIN_GRAMMAR_BLOCK_H__

#include <semantic/fwd-decl.h>
#include <util/arrays.h>

#include "fwd-decl.h"

namespace grammar {

    struct Block {
        Block();

        Block(Block const&) = delete;

        Block(Block&& rhs)
            : _stmts(std::move(rhs._stmts))
            , _funcs(std::move(rhs._funcs))
            , _classes(std::move(rhs._classes))
            , _ctor(std::move(rhs._ctor))
        {}

        util::sref<Constructor const> getCtor() const
        {
            return *this->_ctor;
        }

        semantic::Block compile() const;

        void addStmt(util::sptr<Statement> stmt);
        void addFunc(util::sptr<Function const> func);
        void addClass(util::sptr<Class const> cls);
        void setCtor(misc::position const& pos, std::vector<std::string> params, Block body);
        void acceptElse(misc::position const& else_pos, Block block);
        void acceptCatch(misc::position const& catch_pos, Block block);
    private:
        util::ptrarr<Statement> _stmts;
        util::ptrarr<Function const> _funcs;
        util::ptrarr<Class const> _classes;
        util::sptr<Constructor const> _ctor;
    };

}

#endif /* __STEKIN_GRAMMAR_BLOCK_H__ */
