#ifndef __STEKIN_OUTPUT_FUNCTION_H__
#define __STEKIN_OUTPUT_FUNCTION_H__

#include <util/arrays.h>

#include "node-base.h"

namespace output {

    struct Function {
        virtual ~Function() {}
        Function(Function const&) = delete;

        Function()
            : id(util::uid::next_id())
        {}

        void write(std::ostream& os) const;
        util::sptr<Expression const> callMe(
                misc::position const& pos, util::ptrarr<Expression const> args) const;

        util::sptr<Expression const> callMe(
                misc::position const& pos, util::sptr<Expression const> arg) const
        {
            util::ptrarr<Expression const> args;
            args.append(std::move(arg));
            return this->callMe(pos, std::move(args));
        }

        util::sptr<Expression const> callMe(misc::position const& pos) const
        {
            return this->callMe(pos, util::ptrarr<Expression const>());
        }

        virtual util::sref<Statement const> body() const = 0;
        virtual std::string mangledName() const = 0;
        virtual std::vector<std::string> parameters() const = 0;

        util::uid const id;
    };

}

#endif /* __STEKIN_OUTPUT_FUNCTION_H__ */
