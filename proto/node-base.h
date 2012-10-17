#ifndef __STEKIN_PROTO_NODE_BASE_H__
#define __STEKIN_PROTO_NODE_BASE_H__

#include <vector>

#include <util/pointer.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"

namespace proto {

    struct Expression {
        virtual ~Expression() {}

        virtual void write() const = 0;
        virtual void writeAsPipe() const;

        misc::position const pos;
    protected:
        explicit Expression(misc::position const ps)
            : pos(ps)
        {}
    };

    struct Statement {
        virtual ~Statement() {}

        virtual void write() const = 0;
    protected:
        Statement() {}
    };

}

#endif /* __STEKIN_PROTO_NODE_BASE_H__ */
