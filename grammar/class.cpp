#include <globals.h>
#include <semantic/function.h>

#include "node-base.h"
#include "function.h"
#include "class.h"

using namespace grammar;

util::sptr<semantic::Statement const> Class::compile() const
{
    if (this->base_class.not_nul()) {
        flats::Globals::g.use_class_ext = true;
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

    util::sptr<semantic::Expression const> base(nullptr);
    if (this->base_class.not_nul()) {
        base = this->base_class->reduceAsExpr();
    }
    return util::mkptr(new semantic::Class(
        this->pos, this->name, std::move(base), this->body.compile(), std::move(ctor)));
}
