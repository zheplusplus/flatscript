#include <sstream>

#include <util/string.h>

#include "class.h"
#include "name-mangler.h"

using namespace output;

static std::string const CTOR_TEMPL(
    "function #NAME(#PARAMS) {"
        "if (!(this instanceof #NAME)) {"
            "return new #NAME(#PARAMS)"
        "}"
        "#BODY"
    "}"
);

static std::string constructor(std::string const& name, util::sptr<Constructor const> const& ctor)
{
    if (ctor.nul()) {
        return
            util::replace_all(
            util::replace_all(
            util::replace_all(
                CTOR_TEMPL
                    , "#NAME", name)
                    , "#PARAMS", "")
                    , "#BODY", "")
            ;
    }
    std::ostringstream ctor_os;
    ctor->body->write(ctor_os);
    return
        util::replace_all(
        util::replace_all(
        util::replace_all(
            CTOR_TEMPL
                , "#NAME", name)
                , "#PARAMS", util::join(",", ::formNames(ctor->param_names)))
                , "#BODY", ctor_os.str())
        ;
}

static std::string inherit(std::string const& name)
{
    return
        util::replace_all(
            "$extend(#NAME ,$b);"
            "var $super = #NAME.$super;"
                , "#NAME", name)
    ;
}

static std::string memfuncs(std::string const& name
                          , std::map<std::string, util::sptr<Expression const>> const& fns)
{
    std::vector<std::string> fno;
    for (std::pair<std::string const, util::sptr<Expression const>> const& fn: fns) {
        fno.push_back(name + ".prototype." + fn.first + '=' + fn.second->str() + ';');
    }
    return util::join("", fno);
}

void ClassInitFunc::write(std::ostream& os) const
{
    os <<
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
            this->inherit
                ?  "function #NAME($b){#CONSTRUCTOR #INHERIT #MEMFUNCS #CREATORFN}"
                :  "function #NAME(){#CONSTRUCTOR #MEMFUNCS #CREATORFN}"
                    , "#NAME", output::formClassName(this->name))
                    , "#CONSTRUCTOR", ::constructor(this->name, this->ctor_or_nul))
                    , "#INHERIT", ::inherit(this->name))
                    , "#MEMFUNCS", ::memfuncs(this->name, this->member_funcs))
                    , "#CREATORFN", "return " + this->name)
        ;
}

void ClassInitCall::write(std::ostream& os) const
{
    os <<
        util::replace_all(
        util::replace_all(
        util::replace_all(
            "#NAME = #MANGLED(#BASE);"
                , "#NAME", output::formName(this->name))
                , "#MANGLED", output::formClassName(this->name))
                , "#BASE", this->base_class_or_nul.nul() ? "" : this->base_class_or_nul->str())
        ;
}
