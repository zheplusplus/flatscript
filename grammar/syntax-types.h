#ifndef __STEKIN_PARSER_SYNTAX_TYPE_H__
#define __STEKIN_PARSER_SYNTAX_TYPE_H__

#include <string>
#include <vector>

#include <flowcheck/fwd-decl.h>
#include <report/errors.h>
#include <util/pointer.h>
#include <misc/pos-type.h>

namespace grammar {

    struct OpImage {
        explicit OpImage(char const* image)
            : _img(image)
        {}

        std::string deliver();
    private:
        std::string const _img;
    };

    struct Identifier {
        misc::position const pos;

        Identifier(misc::position const& ps, char const* id_text)
            : pos(ps)
            , _id(id_text)
        {}

        std::string deliver();
    private:
        std::string _id;
    };

    struct Strings {
        explicit Strings(std::string const& init_val)
            : _value(init_val)
        {}

        Strings* append(std::string const& value);
        std::string deliver();
    private:
        std::string _value;
    };

    struct NameList {
        NameList* add(std::string const& name);
        std::vector<std::string> deliver();
    private:
        std::vector<std::string> _names;
    };

    struct ArgList {
        ArgList* add(flchk::Expression const* expr);
        std::vector<util::sptr<flchk::Expression const>> deliver();
    private:
        std::vector<util::sptr<flchk::Expression const>> _args;
    };

    struct DictContent {
        typedef std::pair<util::sptr<flchk::Expression const>
                        , util::sptr<flchk::Expression const>> ItemType;

        DictContent* add(flchk::Expression const* key, flchk::Expression const* value);
        std::vector<ItemType> deliver();
    private:
        std::vector<ItemType> _items;
    };

    struct Pipeline {
        struct PipeBase {
            virtual ~PipeBase() {}
            virtual util::sptr<flchk::PipeBase const> deliverCompile() = 0;

            explicit PipeBase(util::sptr<flchk::Expression const> e)
                : _expr(std::move(e))
            {}
        protected:
            util::sptr<flchk::Expression const> _expr;
        };

        struct PipeMap
            : PipeBase
        {
            explicit PipeMap(util::sptr<flchk::Expression const> expr)
                : PipeBase(std::move(expr))
            {}

            util::sptr<flchk::PipeBase const> deliverCompile();
        };

        struct PipeFilter
            : PipeBase
        {
            explicit PipeFilter(util::sptr<flchk::Expression const> expr)
                : PipeBase(std::move(expr))
            {}

            util::sptr<flchk::PipeBase const> deliverCompile();
        };

        Pipeline* add(util::sptr<PipeBase> pipe);
        std::vector<util::sptr<flchk::PipeBase const>> deliverCompile() const;
    private:
        std::vector<util::sptr<PipeBase>> _pipeline;
    };

}

#endif /* __STEKIN_PARSER_SYNTAX_TYPE_H__ */
