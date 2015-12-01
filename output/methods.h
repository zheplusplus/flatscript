#ifndef __STEKIN_OUTPUT_METHODS_H__
#define __STEKIN_OUTPUT_METHODS_H__

#include <string>
#include <util/pointer.h>

#include "node-base.h"

namespace output {

    namespace method {

        struct _Method {
            virtual ~_Method() {}
            _Method(_Method const&) = delete;
            _Method() = default;

            virtual std::string scheme(std::string const& e) const = 0;
            virtual bool mayThrow(util::sptr<Expression const> const& e) const = 0;

            std::string scheme(util::sptr<Expression const> const& e) const
            {
                return this->scheme(e->str());
            }
        };

        typedef util::sptr<_Method const> _MethodPtr;

        _MethodPtr place();
        _MethodPtr throwExc();
        _MethodPtr callbackExc();
        _MethodPtr asyncCatcher(std::string catcher_func_name);
        _MethodPtr ret();
        _MethodPtr asyncRet();
        _MethodPtr syncPipeRet(util::id pipe_id);

    }

    typedef method::_MethodPtr Method;

}

#endif /* __STEKIN_OUTPUT_METHODS_H__ */
