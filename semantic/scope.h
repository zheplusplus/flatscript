#ifndef __FLSC_SEMANTIC_SCOPE_H__
#define __FLSC_SEMANTIC_SCOPE_H__

#include <output/block.h>
#include <output/methods.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"

namespace semantic {

    struct Scope {
        virtual ~Scope() {}
        Scope(Scope const&) = delete;
        Scope();

        void addStmt(misc::position const& pos, util::sptr<output::Statement const> stmt);
        void checkNotTerminated(misc::position const& pos);
        void terminate(misc::position const& pos);

        bool terminated() const
        {
            return this->_term_pos_or_nul_if_not_term.not_nul();
        }

        util::sref<output::Block> block() const
        {
            return this->_current_block;
        }

        virtual bool inPipe() const = 0;
        virtual bool inCatch() const = 0;
        virtual bool allowSuper() const = 0;

        void setAsyncSpace(misc::position const& pos
                         , std::vector<std::string> const& params
                         , util::sref<output::Block> block);
        virtual util::sref<output::Block> replaceSpace(
                misc::position const& pos, util::sref<output::Block> block);
        virtual void referenceThis(misc::position const& pos) = 0;

        virtual output::Method retMethod(misc::position const& p) = 0;
        virtual output::Method throwMethod() const = 0;
        virtual util::sref<misc::position const> firstReturn() const = 0;

        virtual util::sref<SymbolTable> sym() = 0;

        virtual util::sptr<output::Block> deliver();
    public:
        virtual util::sptr<Scope> makeSyncPipelineScope(util::id pipe_id, bool root);
        virtual util::sptr<Scope> makeAsyncPipelineScope(util::id pipe_id, bool root);
    public:
        static util::sptr<Scope> global();
    protected:
        bool _terminated_err_reported;
        util::sptr<misc::position const> _term_pos_or_nul_if_not_term;
        util::sptr<output::Block> _main_block;
        util::sref<output::Block> _current_block;
    };

}

#endif /* __FLSC_SEMANTIC_SCOPE_H__ */
