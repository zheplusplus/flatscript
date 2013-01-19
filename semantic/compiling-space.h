#ifndef __STEKIN_SEMANTIC_COMPILING_SPACE_H__
#define __STEKIN_SEMANTIC_COMPILING_SPACE_H__

#include <string>
#include <vector>

#include <output/block.h>
#include <util/pointer.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"

namespace semantic {

    struct SymbolTable {
        SymbolTable() = default;
        SymbolTable(SymbolTable const&) = delete;
        virtual ~SymbolTable() {}

        virtual void defName(misc::position const& pos, std::string const& name) = 0;
        virtual void defAsyncParam(misc::position const& pos, std::string const& name) = 0;
        virtual void defConst(misc::position const& pos
                            , std::string const& name
                            , util::sref<Expression const> value) = 0;
        virtual void imported(misc::position const& pos, std::string const& name) = 0;
        virtual void refNames(misc::position const& pos, std::vector<std::string> const& names) = 0;

        virtual util::sref<Expression const> literalOrNul(std::string const& name) const = 0;
        virtual bool isImported(std::string const& name) const = 0;
        virtual util::sptr<output::Expression const> compileRef(misc::position const& pos
                                                              , std::string const& name) = 0;
        virtual void checkDefinition(misc::position const& pos, std::string const& name) const = 0;
    };

    struct BaseCompilingSpace {
        virtual ~BaseCompilingSpace() {}
        BaseCompilingSpace(BaseCompilingSpace const&) = delete;

        explicit BaseCompilingSpace(util::sptr<SymbolTable> symbols);

        BaseCompilingSpace(BaseCompilingSpace&& rhs)
            : _symbols(std::move(rhs._symbols))
            , _main_block(std::move(rhs._main_block))
            , _current_block(rhs._current_block)
        {}

        util::sref<SymbolTable> sym();
        util::sref<output::Block> block() const;

        void setAsyncSpace(misc::position const& pos
                         , std::vector<std::string> const& params
                         , util::sref<output::Block> block);
        void setAsyncSpace(util::sref<output::Block> block);
        virtual void referenceThis() = 0;

        virtual util::sptr<output::Statement const> compileRet();
        virtual util::sptr<output::Statement const> compileRet(
                                            util::sptr<Expression const> const& val);

        virtual util::sptr<output::Block> deliver();
    private:
        util::sptr<SymbolTable> _symbols;
        util::sptr<output::Block> _main_block;
        util::sref<output::Block> _current_block;
    };

    struct CompilingSpace
        : BaseCompilingSpace
    {
        CompilingSpace();
        CompilingSpace(misc::position const& pos
                     , util::sref<SymbolTable> ext_st
                     , std::vector<std::string> const& params);

        CompilingSpace(CompilingSpace&& rhs)
            : BaseCompilingSpace(std::move(rhs))
            , _this_referenced(rhs._this_referenced)
        {}

        void referenceThis();
        util::sptr<output::Block> deliver();
    private:
        bool _this_referenced;
    };

    struct PipelineSpace
        : BaseCompilingSpace
    {
        explicit PipelineSpace(BaseCompilingSpace& ext_space);

        void referenceThis();
        util::sptr<output::Statement const> compileRet();
        util::sptr<output::Statement const> compileRet(util::sptr<Expression const> const& val);
        util::sptr<output::Block> deliver();
    private:
        BaseCompilingSpace& _ext_space;
    };

    struct SubCompilingSpace
        : BaseCompilingSpace
    {
        explicit SubCompilingSpace(BaseCompilingSpace& ext_space);

        void referenceThis();
        util::sptr<output::Statement const> compileRet();
        util::sptr<output::Statement const> compileRet(util::sptr<Expression const> const& val);
    private:
        BaseCompilingSpace& _ext_space;
    };

}

#endif /* __STEKIN_SEMANTIC_COMPILING_SPACE_H__ */
