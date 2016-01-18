#ifndef __STEKIN_OUTPUT_FUNCTION_H__
#define __STEKIN_OUTPUT_FUNCTION_H__

#include <util/arrays.h>

#include "node-base.h"

namespace output {

    struct Function {
        virtual ~Function() {}
        Function(Function const&) = delete;
        Function() = default;

        void write(std::ostream& os) const;
        util::sptr<Expression const> callMe(util::ptrarr<Expression const> args) const;

        util::sptr<Expression const> callMe(util::sptr<Expression const> arg) const
        {
            util::ptrarr<Expression const> args;
            args.append(std::move(arg));
            return this->callMe(std::move(args));
        }

        util::sptr<Expression const> callMe() const
        {
            return this->callMe(util::ptrarr<Expression const>());
        }

        virtual util::sref<Statement const> body() const = 0;
        virtual std::string mangledName() const = 0;
        virtual std::vector<std::string> parameters() const = 0;
    };

}

#endif /* __STEKIN_OUTPUT_FUNCTION_H__ */
