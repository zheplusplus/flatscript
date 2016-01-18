#ifndef __FLSC_OUTPUT_FUNCTION_IMPLEMENTATION_H__
#define __FLSC_OUTPUT_FUNCTION_IMPLEMENTATION_H__

#include <util/uid.h>

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
            : id(util::uid::next_id())
            , _body(new Block)
        {}

        util::sref<Statement const> body() const;
        std::string mangledName() const;

        util::sref<Block> bodyFlow();
    private:
        util::uid const id;
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

    struct AsyncCatchFunc
        : AnonymousCallback
    {
        AsyncCatchFunc(std::string excn, util::uid cid)
            : except_name(std::move(excn))
            , catch_id(cid)
        {}

        std::vector<std::string> parameters() const;

        std::string const except_name;
        util::uid const catch_id;
    };

    struct AsyncCatcherDeprecated
        : AnonymousCallback
    {
        AsyncCatcherDeprecated() = default;

        std::vector<std::string> parameters() const;
    };

    struct ModuleInitFunc
        : Function
    {
        ModuleInitFunc(util::uid i, util::sptr<Statement const> s)
            : module_id(i)
            , stmt(std::move(s))
        {}

        util::sref<Statement const> body() const
        {
            return *this->stmt;
        }

        std::string mangledName() const;
        std::vector<std::string> parameters() const;

        util::uid const module_id;
        util::sptr<Statement const> stmt;

        util::sptr<Expression const> exportArg() const;

        struct InitTarget
            : Expression
        {
            explicit InitTarget(util::uid i)
                : module_id(i)
            {}

            std::string str() const;

            bool mayThrow() const
            {
                return false;
            }

            util::uid const module_id;
        };
    };

}

#endif /* __FLSC_OUTPUT_FUNCTION_IMPLEMENTATION_H__ */
