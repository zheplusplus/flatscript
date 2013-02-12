#ifndef __STEKIN_OUTPUT_FUNCTION_H__
#define __STEKIN_OUTPUT_FUNCTION_H__

#include "fwd-decl.h"
#include "block.h"

namespace output {

    struct Function {
        virtual ~Function() {}
        Function(Function const&) = delete;
        Function() = default;

        void write(std::ostream& os) const;

        virtual util::sref<Statement const> body() const = 0;
        virtual std::string mangledName() const = 0;
        virtual std::vector<std::string> parameters() const = 0;
    };

    struct RegularFunction
        : Function
    {
        RegularFunction(std::string const& func_name
                      , std::vector<std::string> const& p
                      , util::sptr<Statement const> b)
            : name(func_name)
            , params(p)
            , body_stmt(std::move(b))
        {}

        util::sref<Statement const> body() const;
        std::string mangledName() const;
        std::vector<std::string> parameters() const;

        std::string const name;
        std::vector<std::string> const params;
        util::sptr<Statement const> const body_stmt;
    };

    struct RegularAsyncFunction
        : RegularFunction
    {
        RegularAsyncFunction(std::string const& func_name
                           , std::vector<std::string> const& params
                           , int index
                           , util::sptr<Statement const> body)
            : RegularFunction(func_name, params, std::move(body))
            , async_param_index(index)
        {}

        std::vector<std::string> parameters() const;

        int const async_param_index;
    };

    struct RegularAsyncReturnCall
        : Expression
    {
        RegularAsyncReturnCall(misc::position const& pos, util::sptr<Expression const> v)
            : Expression(pos)
            , val(std::move(v))
        {}

        std::string str() const;

        util::sptr<Expression const> const val;
    };

    struct ConditionalCallback
        : Function
    {
        ConditionalCallback()
            : _body(new Block)
        {}

        util::sref<Statement const> body() const;
        std::string mangledName() const;
        std::vector<std::string> parameters() const;

        util::sref<Block> bodyFlow();
    private:
        util::sptr<Block> const _body;
    };

}

#endif /* __STEKIN_OUTPUT_FUNCTION_H__ */
