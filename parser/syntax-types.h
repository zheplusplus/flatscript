#ifndef __STEKIN_PARSER_SYNTAX_TYPE_H__
#define __STEKIN_PARSER_SYNTAX_TYPE_H__

#include <string>
#include <vector>

#include <grammar/fwd-decl.h>
#include <report/errors.h>
#include <util/pointer.h>
#include <misc/pos-type.h>

namespace parser {

    struct OpImage {
        std::string const img;

        explicit OpImage(char const* image)
            : img(image)
        {}
    };

    struct Identifier {
        misc::position const pos;
        std::string const id;

        Identifier(misc::position const& ps, char const* id_text)
            : pos(ps)
            , id(id_text)
        {}
    };

    struct Strings {
        misc::position const pos;

        Strings(misc::position const& p, std::string const& init_val)
            : pos(p)
            , _value(init_val)
        {}

        Strings* append(std::string const& value);
        std::string deliver();
    private:
        std::string _value;
    };

    struct ParamNames {
        ParamNames* add(std::string const& name);
        std::vector<std::string> get() const;
    private:
        std::vector<std::string> _names;
    };

    struct ArgList {
        ArgList* add(grammar::Expression const* expr);
        std::vector<util::sptr<grammar::Expression const>> deliver();
    private:
        std::vector<util::sptr<grammar::Expression const>> _params;
    };

    struct Pipeline {
        struct PipeBase {
            virtual ~PipeBase() {}
            virtual util::sptr<grammar::PipeBase const> deliverCompile() = 0;

            explicit PipeBase(util::sptr<grammar::Expression const> e)
                : _expr(std::move(e))
            {}
        protected:
            util::sptr<grammar::Expression const> _expr;
        };

        struct PipeMap
            : public PipeBase
        {
            explicit PipeMap(util::sptr<grammar::Expression const> expr)
                : PipeBase(std::move(expr))
            {}

            util::sptr<grammar::PipeBase const> deliverCompile();
        };

        struct PipeFilter
            : public PipeBase
        {
            explicit PipeFilter(util::sptr<grammar::Expression const> expr)
                : PipeBase(std::move(expr))
            {}

            util::sptr<grammar::PipeBase const> deliverCompile();
        };

        Pipeline* add(util::sptr<PipeBase> pipe);
        std::vector<util::sptr<grammar::PipeBase const>> deliverCompile() const;
    private:
        std::vector<util::sptr<PipeBase>> _pipeline;
    };

}

#endif /* __STEKIN_PARSER_SYNTAX_TYPE_H__ */
