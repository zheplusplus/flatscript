#ifndef __STEKIN_SEMANTIC_COMPILING_SPACE_H__
#define __STEKIN_SEMANTIC_COMPILING_SPACE_H__

#include <output/block.h>
#include <output/methods.h>
#include <output/function.h>
#include <util/arrays.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"

namespace semantic {

    struct SymbolTable {
        SymbolTable() = default;
        SymbolTable(SymbolTable const&) = delete;
        virtual ~SymbolTable() {}

        virtual void defName(misc::position const& pos, std::string const& name) = 0;
        virtual void defFunc(misc::position const& pos, std::string const& name) = 0;
        virtual void defClass(misc::position const& pos, std::string const& name
                            , std::string const& base_class_name) = 0;
        virtual void defParam(misc::position const& pos, std::string const& name) = 0;
        virtual void defAsyncParam(misc::position const& pos, std::string const& name) = 0;
        virtual void defConst(misc::position const& pos
                            , std::string const& name
                            , util::sref<Expression const> value) = 0;
        virtual void addExternNames(misc::position const& pos
                                  , std::vector<std::string> const& names) = 0;
        virtual void refNames(misc::position const& pos, std::vector<std::string> const& names) = 0;

        virtual util::sref<Expression const> literalOrNul(std::string const& name) const = 0;

        virtual util::sptr<output::Expression const> compileRef(misc::position const& pos
                                                              , std::string const& name) = 0;
        virtual std::set<std::string> localNames() const = 0;
    };

    struct BaseCompilingSpace {
        virtual ~BaseCompilingSpace() {}
        BaseCompilingSpace(BaseCompilingSpace const&) = delete;

        BaseCompilingSpace(util::sptr<SymbolTable> symbols, bool allow_super);

        explicit BaseCompilingSpace(util::sptr<SymbolTable> symbols)
            : BaseCompilingSpace(std::move(symbols), false)
        {}

        util::sref<SymbolTable> sym();
        util::sptr<output::Expression const> makeReference(
            misc::position const& pos, std::string const& name);
        void addStmt(misc::position const& pos, util::sptr<output::Statement const> stmt);
        void checkNotTerminated(misc::position const& pos);
        util::sref<output::Block> block() const;
        void terminate(misc::position const& pos);
        bool terminated() const;

        virtual bool inPipe() const = 0;
        virtual bool inCatch() const = 0;
        virtual bool allowSuper() const { return this->_allow_super; }
        void allowSuper(bool allow) { this->_allow_super = allow; }

        void setAsyncSpace(misc::position const& pos
                         , std::vector<std::string> const& params
                         , util::sref<output::Block> block);
        virtual util::sref<output::Block> replaceSpace(
                misc::position const& pos, util::sref<output::Block> block);
        virtual void referenceThis() = 0;

        virtual output::Method retMethod(misc::position const& p) const;
        virtual output::Method throwMethod() const;

        virtual util::sptr<output::Block> deliver();
    private:
        bool _allow_super;
        bool _terminated_err_reported;
        util::sptr<misc::position const> _term_pos_or_nul_if_not_term;
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
                     , std::vector<std::string> const& params
                     , bool allow_super=false);

        bool inPipe() const { return false; }
        bool inCatch() const { return false; }

        void referenceThis();
        util::sptr<output::Block> deliver();
    private:
        bool _this_referenced;
    };

    struct RegularAsyncCompilingSpace
        : CompilingSpace
    {
        RegularAsyncCompilingSpace(misc::position const& pos
                                 , util::sref<SymbolTable> ext_st
                                 , std::vector<std::string> const& params
                                 , bool allow_super=false);

        output::Method retMethod(misc::position const&) const;
        output::Method throwMethod() const;
        util::sptr<output::Block> deliver();
        util::sref<output::Block> replaceSpace(
                misc::position const& pos, util::sref<output::Block> block);

        misc::position const compile_pos;
    };

    struct SubCompilingSpace
        : BaseCompilingSpace
    {
        SubCompilingSpace(util::sptr<SymbolTable> symbols, BaseCompilingSpace& ext_space)
            : BaseCompilingSpace(std::move(symbols))
            , _ext_space(ext_space)
        {}

        SubCompilingSpace(BaseCompilingSpace& ext_space);

        bool inPipe() const;
        bool inCatch() const;
        bool allowSuper() const;
        output::Method retMethod(misc::position const& p) const;
        output::Method throwMethod() const;
        util::sref<output::Block> replaceSpace(
                misc::position const& pos, util::sref<output::Block> block);
        void referenceThis();
    private:
        BaseCompilingSpace& _ext_space;
    };

    struct RegularSubCompilingSpace
        : SubCompilingSpace
    {
        explicit RegularSubCompilingSpace(BaseCompilingSpace& ext_space);
    };

    struct PipelineSpace
        : SubCompilingSpace
    {
        explicit PipelineSpace(BaseCompilingSpace& ext_space);

        bool inPipe() const { return true; }
        output::Method retMethod(misc::position const& p) const;
    };

    struct AsyncPipelineSpace
        : PipelineSpace
    {
        explicit AsyncPipelineSpace(BaseCompilingSpace& ext_space)
            : PipelineSpace(ext_space)
        {}

        util::sptr<output::Block> deliver();
    };

    struct BranchCompilingSpace
        : SubCompilingSpace
    {
        explicit BranchCompilingSpace(BaseCompilingSpace& ext_space)
            : SubCompilingSpace(ext_space)
        {}
    };

    struct CatcherSpace
        : SubCompilingSpace
    {
        explicit CatcherSpace(BaseCompilingSpace& ext_space)
            : SubCompilingSpace(ext_space)
        {}

        bool inCatch() const { return true; }
    };

    struct AsyncTrySpace
        : SubCompilingSpace
    {
        AsyncTrySpace(BaseCompilingSpace& ext_space, util::sref<output::Function const> cf)
            : SubCompilingSpace(ext_space)
            , catch_func(cf)
        {}

        output::Method throwMethod() const;
        util::sref<output::Block> replaceSpace(
                misc::position const& pos, util::sref<output::Block> block);

        util::sref<output::Function const> const catch_func;
    };

}

#endif /* __STEKIN_SEMANTIC_COMPILING_SPACE_H__ */
