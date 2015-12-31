#ifndef __FLSC_SEMANTIC_SCOPE_IMPLEMENTATION_H__
#define __FLSC_SEMANTIC_SCOPE_IMPLEMENTATION_H__

#include <output/function.h>

#include "scope.h"
#include "symbol-table.h"

namespace semantic {

    struct DomainScope
        : Scope
    {
        struct ClassInitFunc {
            ClassInitFunc(misc::position const& p
                        , std::string cn
                        , bool inh
                        , util::sptr<output::Constructor const> ct
                        , std::map<std::string, util::sref<Function const>> mf)
                : pos(p)
                , class_name(std::move(cn))
                , ctor(std::move(ct))
                , memfns(std::move(mf))
                , inherit(inh)
            {}

            ClassInitFunc(ClassInitFunc const&) = delete;

            ClassInitFunc(ClassInitFunc&& rhs)
                : pos(rhs.pos)
                , class_name(std::move(rhs.class_name))
                , ctor(std::move(rhs.ctor))
                , memfns(std::move(rhs.memfns))
                , inherit(rhs.inherit)
            {}

            misc::position pos;
            std::string class_name;
            util::sptr<output::Constructor const> ctor;
            std::map<std::string, util::sref<Function const>> memfns;
            bool inherit;
        };

        void addClass(misc::position const& pos
                    , std::string const& class_name
                    , util::sptr<output::Expression const> base_class
                    , util::sptr<output::Constructor const> ctor
                    , std::map<std::string, util::sref<Function const>> memfns);

        output::Method breakMethod(misc::position const& p);
        output::Method continueMethod(misc::position const& p);
        util::uid scopeId() const;
        bool hasBreak() const { return false; }
        util::sptr<output::Block> deliver();
    protected:
        std::vector<ClassInitFunc> _class_inits;
    };

    struct BasicFunctionScope
        : DomainScope
    {
        BasicFunctionScope(misc::position const& pos
                         , util::sref<SymbolTable> ext_st
                         , std::vector<std::string> const& params
                         , bool allow_super)
            : _sym(pos, ext_st, params)
            , _first_return_or_nul_if_not_returned(nullptr)
            , _allow_super(allow_super)
            , _this_referenced(false)
        {}

        bool inPipe() const { return false; }
        bool inCatch() const { return false; }
        bool allowSuper() const  { return this->_allow_super; }

        void allowSuper(bool allow)
        {
            this->_allow_super = allow;
        }

        void referenceThis(misc::position const&)
        {
            this->_this_referenced = true;
        }

        util::sref<SymbolTable> sym()
        {
            return util::mkref(this->_sym);
        }

        util::sref<misc::position const> firstReturn() const
        {
            return *this->_first_return_or_nul_if_not_returned;
        }

        util::sptr<output::Block> deliver();
    protected:
        RegularSymbolTable _sym;
        util::sptr<misc::position const> _first_return_or_nul_if_not_returned;
        bool _allow_super;
        bool _this_referenced;
    };

    struct SyncFunctionScope
        : BasicFunctionScope
    {
        SyncFunctionScope(misc::position const& pos
                        , util::sref<SymbolTable> ext_st
                        , std::vector<std::string> const& params
                        , bool allow_super)
            : BasicFunctionScope(pos, ext_st, params, allow_super)
        {}

        output::Method retMethod(misc::position const& p);
        output::Method throwMethod() const;
    };

    struct RegularAsyncFuncScope
        : BasicFunctionScope
    {
        RegularAsyncFuncScope(misc::position const& pos
                            , util::sref<SymbolTable> ext_st
                            , std::vector<std::string> const& params
                            , bool allow_super);

        output::Method retMethod(misc::position const& p);
        output::Method throwMethod() const;
        util::sptr<output::Block> deliver();
        util::sref<output::Block> replaceSpace(
                misc::position const& pos, util::sref<output::Block> block);

        misc::position const ret_pos;
    };

    struct SubScope
        : Scope
    {
        explicit SubScope(util::sref<Scope> parent_scope)
            : _parent_scope(parent_scope)
        {}

        bool inPipe() const
        {
            return this->_parent_scope->inPipe();
        }

        bool inCatch() const
        {
            return this->_parent_scope->inCatch();
        }

        bool allowSuper() const
        {
            return this->_parent_scope->allowSuper();
        }

        output::Method retMethod(misc::position const& p)
        {
            return this->_parent_scope->retMethod(p);
        }

        output::Method throwMethod() const
        {
            return this->_parent_scope->throwMethod();
        }

        output::Method breakMethod(misc::position const& p)
        {
            return this->_parent_scope->breakMethod(p);
        }

        output::Method continueMethod(misc::position const& p)
        {
            return this->_parent_scope->continueMethod(p);
        }

        util::sref<misc::position const> firstReturn() const
        {
            return this->_parent_scope->firstReturn();
        }

        bool hasBreak() const { return false; }

        util::sref<output::Block> replaceSpace(
                misc::position const& pos, util::sref<output::Block> block)
        {
            return this->_parent_scope->replaceSpace(pos, block);
        }

        void referenceThis(misc::position const& pos)
        {
            return this->_parent_scope->referenceThis(pos);
        }

        void addClass(misc::position const& pos
                    , std::string const& class_name
                    , util::sptr<output::Expression const>
                    , util::sptr<output::Constructor const>
                    , std::map<std::string, util::sref<Function const>>);

        util::uid scopeId() const
        {
            return this->_parent_scope->scopeId();
        }
    protected:
        util::sref<Scope> const _parent_scope;
    };

    struct BranchingSubScope
        : SubScope
    {
        explicit BranchingSubScope(util::sref<Scope> parent_scope)
            : SubScope(parent_scope)
            , _sym(parent_scope->sym())
        {}

        util::sref<SymbolTable> sym()
        {
            return util::mkref(this->_sym);
        }
    private:
        SubSymbolTable _sym;
    };

    struct CatchScope
        : BranchingSubScope
    {
        explicit CatchScope(util::sref<Scope> parent_scope)
            : BranchingSubScope(parent_scope)
        {}

        bool inCatch() const { return true; }
    };

    struct AsyncTryScope
        : BranchingSubScope
    {
        AsyncTryScope(util::sref<Scope> parent_scope, util::sref<output::Function const> cf)
            : BranchingSubScope(parent_scope)
            , catch_func(cf)
        {}

        output::Method throwMethod() const;
        util::sref<output::Block> replaceSpace(
                misc::position const& pos, util::sref<output::Block> block);

        util::sref<output::Function const> const catch_func;
    };

}

#endif /* __FLSC_SEMANTIC_SCOPE_IMPLEMENTATION_H__ */
