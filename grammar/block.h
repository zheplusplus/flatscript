#ifndef __STEKIN_GRAMMAR_BLOCK_H__
#define __STEKIN_GRAMMAR_BLOCK_H__

#include <util/arrays.h>
#include <semantic/block.h>

#include "node-base.h"
#include "function.h"

namespace grammar {

    struct Constructor;

    struct Block
        : Statement
    {
        Block();

        util::sref<Constructor const> getCtor() const
        {
            return *this->_ctor;
        }

        util::sptr<semantic::Statement const> compile() const
        {
            return this->compileToBlock();
        }

        util::sptr<semantic::Block const> compileToBlock() const;

        void addStmt(util::sptr<Statement> stmt)
        {
            this->_stmts.append(std::move(stmt));
        }

        void addFunc(util::sptr<Function const> func)
        {
            this->_funcs.append(std::move(func));
        }

        void setCtor(misc::position const& pos, std::vector<std::string> params
                   , util::sptr<Block const> body, bool super_init
                   , std::vector<util::sptr<Expression const>> super_ctor_args);
        void acceptElse(misc::position const& else_pos, util::sptr<Statement const> block);
        void acceptCatch(misc::position const& catch_pos, util::sptr<Statement const> block,
                         std::string except_name);
    private:
        util::ptrarr<Statement> _stmts;
        util::ptrarr<Function const> _funcs;
        util::sptr<Constructor const> _ctor;
    };

}

#endif /* __STEKIN_GRAMMAR_BLOCK_H__ */
