#include <globals.h>
#include <semantic/function.h>

#include "node-base.h"
#include "function.h"
#include "class.h"

using namespace grammar;

util::sptr<semantic::Class const> Class::compile() const
{
    if (!this->base_class_name.empty()) {
        stekin::Globals::g.use_class_ext = true;
    }
    util::sptr<semantic::Constructor> ctor(nullptr);
    if (this->body.getCtor().not_nul()) {
        util::ptrarr<semantic::Expression const> reduced_super_ctor_args;
        for (auto const& a: this->body.getCtor()->super_ctor_args) {
            reduced_super_ctor_args.append(a->reduceAsExpr());
        }
        ctor = util::mkptr(new semantic::Constructor(
                    this->body.getCtor()->pos, this->body.getCtor()->param_names
                  , this->body.getCtor()->body.compile(), this->name
                  , this->body.getCtor()->super_init , std::move(reduced_super_ctor_args)));
    }
    return util::mkptr(new semantic::Class(
                this->pos, this->name, this->base_class_name
              , this->body.compile(), std::move(ctor)));
}
