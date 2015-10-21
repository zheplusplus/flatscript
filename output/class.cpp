#include <sstream>

#include <util/string.h>

#include "class.h"
#include "name-mangler.h"

using namespace output;

static std::string constructor(std::string const& name, util::sptr<Constructor const> const& ctor)
{
    if (ctor.nul()) {
        return "function " + name + "() {}";
    }
    std::ostringstream ctor_os;
    ctor->body->write(ctor_os);
    return
        util::replace_all(
        util::replace_all(
        util::replace_all(
            "function #NAME(#PARAMS) {#BODY}"
                , "#NAME", name)
                , "#PARAMS", util::join(",", ::formNames(ctor->param_names)))
                , "#BODY", ctor_os.str())
        ;
}

static std::string inherit(std::string const& name, util::sptr<Expression const> const& base)
{
    if (base.nul()) {
        return "";
    }
    return "$extend(" + name + "," + base->str() + ");";
}

static std::string memfuncs(std::string const& name
                          , std::map<std::string, util::sptr<Lambda const>> const& fns)
{
    std::vector<std::string> fno;
    for (std::pair<std::string const, util::sptr<Lambda const>> const& fn: fns) {
        fno.push_back(name + ".prototype." + fn.first + '=' + fn.second->str() + ';');
    }
    return util::join("", fno);
}

static std::string creator(std::string const& name, util::sptr<Constructor const> const& ctor)
{
    return
        util::replace_all(
        util::replace_all(
            "var $super=#NAME.$super;"
            "function create(#PARAMS) {"
                "return new #NAME(#PARAMS);"
            "}"
            "create.$class = #NAME;"
            "return create;"
                , "#NAME", name)
                , "#PARAMS", ctor.nul() ? "" : util::join(",", ::formNames(ctor->param_names)))
        ;
}

void Class::write(std::ostream& os) const
{
    os << output::formName(this->name)
       <<
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
            "=(function(){#CONSTRUCTOR #INHERIT #MEMFUNCS #CREATORFN})();"
                , "#CONSTRUCTOR", ::constructor(this->name, this->ctor_or_nul))
                , "#INHERIT", ::inherit(this->name, this->base_class_or_nul))
                , "#MEMFUNCS", ::memfuncs(this->name, this->member_funcs))
                , "#CREATORFN", ::creator(this->name, this->ctor_or_nul))
        ;
}
