#ifndef __STEKIN_GRAMMAR_REDUCING_ENVIRONMENT_H__
#define __STEKIN_GRAMMAR_REDUCING_ENVIRONMENT_H__

#include <string>
#include <vector>

namespace grammar {

    struct BaseReducingEnv {
        virtual ~BaseReducingEnv() {}

        virtual bool inPipe() const = 0;
    };

    struct ExprReducingEnv
        : BaseReducingEnv
    {
        bool inPipe() const { return false; }
    };

    struct ArgReducingEnv
        : BaseReducingEnv
    {
        explicit ArgReducingEnv(BaseReducingEnv const& ext_env)
            : _external_env(ext_env)
            , _async_index(-1)
        {}

        bool inPipe() const;
        void setAsync(misc::position const& pos, int index, std::vector<std::string> const& ap);
        bool isAsync() const;
        int asyncIndex() const;
        std::vector<std::string> const& asyncParams() const;

    private:
        BaseReducingEnv const& _external_env;
        int _async_index;
        std::vector<std::string> _async_params;
    };

    struct PipeReducingEnv
        : BaseReducingEnv
    {
        bool inPipe() const { return true; }
    };

}

#endif /* __STEKIN_GRAMMAR_REDUCING_ENVIRONMENT_H__ */
