#ifndef __STEKIN_OUTPUT_NODE_BASE_H__
#define __STEKIN_OUTPUT_NODE_BASE_H__

#include <ostream>

namespace output {

    struct Expression {
        virtual ~Expression() {}

        virtual std::string str() const = 0;
        virtual bool mayThrow() const = 0;

        Expression() = default;
        Expression(Expression const&) = delete;
    };

    struct Statement {
        virtual ~Statement() {}

        virtual void write(std::ostream& os) const = 0;
        virtual int count() const { return 1; };
        virtual bool mayThrow() const = 0;

        Statement() = default;
        Statement(Statement const&) = delete;
    };

}

#endif /* __STEKIN_OUTPUT_NODE_BASE_H__ */
