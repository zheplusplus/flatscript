#ifndef __STEKIN_FLOWCHECK_FILTER_H__
#define __STEKIN_FLOWCHECK_FILTER_H__

#include <string>
#include <vector>

#include <misc/pos-type.h>

#include "fwd-decl.h"
#include "accumulator.h"

namespace flchk {

    struct Filter {
        Filter() = default;
        Filter(Filter const&) = delete;

        virtual ~Filter() {}
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
                     , util::sptr<Filter> consequence
                     , util::sptr<Filter> alternative);

        void addBranch(misc::position const& pos
                     , util::sptr<Expression const> predicate
                     , util::sptr<Filter> consequence);

        void addBranchAlterOnly(misc::position const& pos
                              , util::sptr<Expression const> predicate
                              , util::sptr<Filter> alternative);
    public:
        virtual void defName(misc::position const& pos
                           , std::string const& name
                           , util::sptr<Expression const> init) = 0;
        virtual void defFunc(misc::position const& pos
                           , std::string const& name
                           , std::vector<std::string> const& param_names
                           , util::sptr<Filter> body) = 0;
    public:
        virtual util::sref<SymbolTable> getSymbols() = 0;
    public:
        util::sptr<proto::Statement const> compile();
        bool hintReturnVoid() const;
    protected:
        Accumulator _accumulator;
    };

}

#endif /* __STEKIN_FLOWCHECK_FILTER_H__ */
