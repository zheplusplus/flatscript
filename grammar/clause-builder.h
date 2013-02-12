#ifndef __STEKIN_GRAMMAR_CLAUSE_BUILDER_H__
#define __STEKIN_GRAMMAR_CLAUSE_BUILDER_H__

#include "automation-base.h"
#include "block.h"

namespace grammar {

    struct ClauseBuilder {
        ClauseBuilder();

        ClauseBuilder(ClauseBuilder const&) = delete;

        void addArith(int indent_len
                    , misc::position const& pos
                    , std::vector<util::sptr<Token>> const& sequence);
        void addReturn(int indent_len
                     , misc::position const& pos
                     , std::vector<util::sptr<Token>> const& sequence);
        void addImport(int indent_len
                     , misc::position const& pos
                     , std::vector<std::string> const& names);
        void addExport(int indent_len
                     , misc::position const& pos
                     , std::vector<std::string> const& names
                     , std::vector<util::sptr<Token>> const& sequence);
        void addIfnot(int indent_len
                    , misc::position const& pos
                    , std::vector<util::sptr<Token>> const& sequence);

        semantic::Block buildAndClear();
    private:
        bool _shrinkTo(int level, misc::position const& pos);
        bool _prepareLevel(int level, misc::position const& pos, std::string const& token);
        void _pushSequence(misc::position const& pos
                         , std::vector<util::sptr<Token>> const& sequence);

        Block* _global;
        std::vector<util::sptr<ClauseBase>> _clauses;
    };

}

#endif /* __STEKIN_GRAMMAR_CLAUSE_BUILDER_H__ */
