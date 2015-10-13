#include <report/errors.h>

#include "class.h"
#include "function.h"
#include "compiling-space.h"

using namespace semantic;

util::sptr<output::Class const> Class::compile(util::sref<SymbolTable> st) const
{
    util::sptr<output::Class::Constructor const> ct(nullptr);
    if (this->ctor_or_nul.not_nul()) {
        CompilingSpace ctor_space(pos, st, this->ctor_or_nul->param_names, true);
        this->ctor_or_nul->body.compile(ctor_space);
        ct.reset(new output::Class::Constructor(this->ctor_or_nul->param_names, ctor_space.deliver()));
    }

    util::sptr<output::Expression const> base_class(
            this->base_class_name.empty()
          ? util::sptr<output::Expression const>(nullptr)
          : st->compileRef(this->pos, this->base_class_name));

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
