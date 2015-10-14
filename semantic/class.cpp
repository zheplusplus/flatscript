#include <report/errors.h>

#include "class.h"
#include "function.h"
#include "stmt-nodes.h"
#include "expr-nodes.h"
#include "compiling-space.h"

using namespace semantic;

static util::sptr<Statement const> superInitCall(
        misc::position const& pos, bool call_super
      , std::string const& class_name, util::ptrarr<Expression const> super_ctor_args)
{
    if (!call_super) {
        return util::sptr<Statement const>(nullptr);
    }
    return util::mkptr(new Arithmetics(pos, util::mkptr(
                    new SuperConstructorCall(pos, class_name, std::move(super_ctor_args)))));
}

Constructor::Constructor(misc::position const& ps, std::vector<std::string> params
                       , Block b, std::string const& class_name, bool si
                       , util::ptrarr<Expression const> super_ctor_args)
    : pos(ps)
    , param_names(std::move(params))
    , super_init(::superInitCall(ps, si, class_name, std::move(super_ctor_args)))
    , body(std::move(b))
{}

util::sptr<output::Constructor const> Constructor::compile(
                    util::sref<SymbolTable> st, bool has_base_class) const
{
    if (has_base_class && !this->superInit()) {
        error::contructorNotCallSuper(this->pos);
    } else if (!has_base_class && this->superInit()) {
        error::noSuperClass(this->pos);
    }

    CompilingSpace ctor_space(pos, st, this->param_names);
    if (has_base_class) {
        ctor_space.referenceThis();
    }
    if (this->super_init.not_nul()) {
        this->super_init->compile(ctor_space);
    }
    ctor_space.allowSuper(has_base_class);
    this->body.compile(ctor_space);
    return util::mkptr(new output::Constructor(this->param_names, ctor_space.deliver()));
}

util::sptr<output::Class const> Class::compile(util::sref<SymbolTable> st) const
{
    util::sptr<output::Constructor const> ct(nullptr);
    if (this->ctor_or_nul.not_nul()) {
        ct = this->ctor_or_nul->compile(st, this->hasBaseClass());
    } else if (this->hasBaseClass()) {
        error::contructorNotCallSuper(this->pos);
    }

    util::sptr<output::Expression const> base_class(
            this->hasBaseClass()
          ? st->compileRef(this->pos, this->base_class_name)
          : util::sptr<output::Expression const>(nullptr));

    util::sptr<output::Block const> class_body(new output::Block);
    std::map<std::string, misc::position> memfn_defs_pos;
    std::map<std::string, util::sptr<output::Lambda const>> memfuncs;
    this->body.getFuncs().iter([&](util::sptr<Function const> const& func, int)
                               {
                                   if (memfn_defs_pos.find(func->name) != memfn_defs_pos.end()) {
                                       return error::duplicateMemFunc(memfn_defs_pos[func->name]
                                                                    , func->pos, func->name);
                                   }
                                   memfn_defs_pos.insert(std::make_pair(func->name, func->pos));
                                   memfuncs.insert(std::make_pair(
                                        func->name, util::mkptr(new output::Lambda(
                                               func->pos, func->param_names
                                             , func->compileAsMemberFunc(st), false))));
                               });
    return util::mkptr(new output::Class(
            this->name, std::move(base_class), std::move(memfuncs), std::move(ct)));
}
