#ifndef __STEKIN_GRAMMAR_CLAUSE_BUILDER_H__
#define __STEKIN_GRAMMAR_CLAUSE_BUILDER_H__

#include "tokens.h"

namespace grammar {

    struct ClauseBuilder {
        ClauseBuilder();

        ClauseBuilder(ClauseBuilder const&) = delete;

        void addArith(int indent, misc::position const& pos, std::vector<util::sptr<Token>> seq);
        util::sptr<semantic::Statement const> buildAndClear();
    private:
        bool _shrinkTo(int level, misc::position const& pos);
        bool _prepareLevel(int level, misc::position const& pos, std::string const& token);

        util::sref<Statement> _global;
        std::vector<util::sptr<ClauseBase>> _clauses;
    };

}

#endif /* __STEKIN_GRAMMAR_CLAUSE_BUILDER_H__ */
