#ifndef __FLSC_OUTPUT_FUNCTION_IMPLEMENTATION_H__
#define __FLSC_OUTPUT_FUNCTION_IMPLEMENTATION_H__

#include "function.h"
#include "block.h"

namespace output {

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

    struct AnonymousCallback
        : Function
    {
        AnonymousCallback()
            : _body(new Block)
        {}

        util::sref<Statement const> body() const;
        std::string mangledName() const;

        util::sref<Block> bodyFlow();
    private:
        util::sptr<Block> const _body;
    };

    struct ConditionalCallback
        : AnonymousCallback
    {
        ConditionalCallback() = default;

        std::vector<std::string> parameters() const;
    };

    struct NoParamCallback
        : AnonymousCallback
    {
        NoParamCallback() = default;

        std::vector<std::string> parameters() const;
    };

    struct AsyncCatcher
        : AnonymousCallback
    {
        AsyncCatcher() = default;

        std::vector<std::string> parameters() const;
    };

}

#endif /* __FLSC_OUTPUT_FUNCTION_IMPLEMENTATION_H__ */
