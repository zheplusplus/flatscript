#ifndef __STEKIN_OUTPUT_NODE_BASE_H__
#define __STEKIN_OUTPUT_NODE_BASE_H__

#include <ostream>

#include <util/pointer.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"

namespace output {

    struct Expression {
        virtual ~Expression() {}

        virtual std::string str() const = 0;
        virtual std::string strAsProp() const;

        misc::position const pos;

        explicit Expression(misc::position const ps)
            : pos(ps)
        {}

        Expression(Expression const&) = delete;
    };

    struct Statement {
        virtual ~Statement() {}

        virtual void write(std::ostream& os) const = 0;
        virtual int count() const { return 1; };

        Statement() = default;
        Statement(Statement const&) = delete;
    };

}

#endif /* __STEKIN_OUTPUT_NODE_BASE_H__ */
