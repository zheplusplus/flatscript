#ifndef __STEKIN_GRAMMAR_CLAUSE_BUILDER_H__
#define __STEKIN_GRAMMAR_CLAUSE_BUILDER_H__

#include <vector>
#include <string>

#include <semantic/fwd-decl.h>
#include <util/pointer.h>
#include <misc/pos-type.h>

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

        void addFunction(int indent_len
                       , misc::position const& pos
                       , std::string const& name
                       , std::vector<std::string> const& params);
        void addIf(int indent_len
                 , misc::position const& pos
                 , std::vector<util::sptr<Token>> const& sequence);
        void addIfnot(int indent_len
                    , misc::position const& pos
                    , std::vector<util::sptr<Token>> const& sequence);
        void addElse(int indent_len, misc::position const& pos);

        util::sptr<semantic::Filter> buildAndClear();
    private:
        bool _shrinkTo(int level, misc::position const& pos);
        bool _prepareLevel(int level, misc::position const& pos, std::string const& token);
        void _pushSequence(misc::position const& pos
                         , std::vector<util::sptr<Token>> const& sequence);

        Block _global;
        std::vector<util::sptr<ClauseBase>> _clauses;
    };

}

#endif /* __STEKIN_GRAMMAR_CLAUSE_BUILDER_H__ */
