#ifndef __STEKIN_SEMANTIC_FILTER_H__
#define __STEKIN_SEMANTIC_FILTER_H__

#include "fwd-decl.h"
#include "block.h"

namespace semantic {

    struct Filter {
        Filter()
            : _error_reported(false)
            , _term_pos_or_nul_if_not_term(nullptr)
        {}

        Filter(Filter const&) = delete;

        void addReturn(misc::position const& pos, util::sptr<Expression const> ret_val);
        void addReturnNothing(misc::position const& pos);
        void addArith(misc::position const& pos, util::sptr<Expression const> expr);
        void addImport(misc::position const& pos, std::vector<std::string> const& names);
        void addExport(misc::position const& pos
                     , std::vector<std::string> const& export_point
                     , util::sptr<Expression const> value);
        void addAttrSet(misc::position const& pos
                      , util::sptr<Expression const> set_point
                      , util::sptr<Expression const> value);

        void addBranch(misc::position const& pos
                     , util::sptr<Expression const> predicate
                     , util::sptr<Filter> consequence
                     , util::sptr<Filter> alternative);

        void addBranch(misc::position const& pos
                     , util::sptr<Expression const> predicate
                     , util::sptr<Filter> consequence);

        void addBranchAlterOnly(misc::position const& pos
                              , util::sptr<Expression const> predicate
                              , util::sptr<Filter> alternative);

        void defName(misc::position const& pos
                   , std::string const& name
                   , util::sptr<Expression const> init);
        void defFunc(util::sptr<Function const> func);

        Block deliver();
    private:
        void _checkBranchesTermination(misc::position const& pos
                                     , util::sptr<Filter> const& consequence
                                     , util::sptr<Filter> const& alternative);
        void _checkNotTerminated(misc::position const& pos);
        void _reportTerminated(misc::position const& pos);
        bool _terminated() const;
        void _setTerminated(misc::position const& pos);

        Block _block;
        bool _error_reported;
        util::sptr<misc::position> _term_pos_or_nul_if_not_term;
    };

}

#endif /* __STEKIN_SEMANTIC_FILTER_H__ */
