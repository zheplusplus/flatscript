#ifndef __STEKIN_GRAMMAR_NODE_BASE_H__
#define __STEKIN_GRAMMAR_NODE_BASE_H__

#include <flowcheck/fwd-decl.h>
#include <util/pointer.h>
#include <misc/pos-type.h>

namespace grammar {

    struct Statement {
        misc::position const pos;

        virtual void compile(util::sref<flchk::Filter> filter) = 0;

        virtual ~Statement() {}
    protected:
        explicit Statement(misc::position const& ps)
            : pos(ps)
        {}

        Statement(Statement const&) = delete;
    };

}

#endif /* __STEKIN_GRAMMAR_NODE_BASE_H__ */
