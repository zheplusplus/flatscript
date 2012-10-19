#ifndef __STEKIN_FLOWCHECK_ACCUMULATOR_H__
#define __STEKIN_FLOWCHECK_ACCUMULATOR_H__

#include <string>
#include <vector>

#include <util/pointer.h>
#include <misc/pos-type.h>

#include "block.h"

namespace flchk {

    struct Accumulator {
        Accumulator()
            : _contains_void_return(false)
            , _error_reported(false)
            , _term_pos_or_nul_if_not_term(nullptr)
        {}

        Accumulator(Accumulator const&) = delete;

        Accumulator(Accumulator&& rhs)
            : _block(std::move(rhs._block))
            , _contains_void_return(rhs._contains_void_return)
            , _error_reported(rhs._error_reported)
            , _term_pos_or_nul_if_not_term(std::move(rhs._term_pos_or_nul_if_not_term))
        {}
    public:
        void addReturn(misc::position const& pos, util::sptr<Expression const> ret_val);
        void addReturnNothing(misc::position const& pos);
        void addArith(misc::position const& pos, util::sptr<Expression const> expr);
        void addImport(misc::position const& pos, std::vector<std::string> const& names);
        void addAttrSet(misc::position const& pos
                      , util::sptr<Expression const> set_point
                      , util::sptr<Expression const> value);

        void addBranch(misc::position const& pos
                     , util::sptr<Expression const> predicate
                     , Accumulator consequence
                     , Accumulator alternative);

        void addBranch(misc::position const& pos
                     , util::sptr<Expression const> predicate
                     , Accumulator consequence);

        void addBranchAlterOnly(misc::position const& pos
                              , util::sptr<Expression const> predicate
                              , Accumulator alternative);

        void addBlock(Accumulator b);
    public:
        void defName(misc::position const& pos
                   , std::string const& name
                   , util::sptr<Expression const> init);

        util::sref<Function const> defFunc(misc::position const& pos
                                         , std::string const& name
                                         , std::vector<std::string> const& param_names
                                         , util::sptr<Filter> body);
    public:
        util::sptr<proto::Statement const> compileBlock(util::sref<SymbolTable> st) const;
        bool hintReturnVoid() const;
    private:
        void _setTerminatedByVoidReturn(misc::position const& pos);
        void _setTerminatedNotByVoidReturn(misc::position const& pos);
        void _setTerminationBySubAccumulator(Accumulator const& sub);

        static void _checkBranchesTermination(Accumulator const& consequence
                                            , Accumulator const& alternative);
        void _checkNotTerminated(misc::position const& pos);

        void _reportTerminated(misc::position const& pos);

        bool _terminated() const;
        void _setSelfTerminated(Accumulator term);
    private:
        Block _block;
        bool _contains_void_return;
        bool _error_reported;
        util::sptr<misc::position> _term_pos_or_nul_if_not_term;
    };

}

#endif /* __STEKIN_FLOWCHECK_ACCUMULATOR_H__ */
