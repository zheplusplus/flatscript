#include <algorithm>

#include <output/function.h>
#include <output/stmt-nodes.h>
#include <output/expr-nodes.h>
#include <report/errors.h>

#include "stmt-nodes.h"
#include "function.h"
#include "compiling-space.h"

using namespace semantic;

void Arithmetics::compile(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> cexpr(expr->compile(space));
    space.addStmt(pos, util::mkptr(new output::Arithmetics(std::move(cexpr))));
}

bool Arithmetics::isAsync() const
{
    return expr->isAsync();
}

void Branch::compile(BaseCompilingSpace& space) const
{
    if (predicate->isLiteral(space.sym())) {
        Block const& block(predicate->boolValue(space.sym()) ? consequence : alternative);
        BranchCompilingSpace sub_space(space);
        block.compile(sub_space);
        return space.addStmt(pos, sub_space.deliver());
    }
    util::sptr<output::Expression const> compiled_pred(predicate->compile(space));
    BranchCompilingSpace consq_space(space);
    consequence.compile(consq_space);
    BranchCompilingSpace alter_space(space);
    alternative.compile(alter_space);

    space.addStmt(pos, util::mkptr(new output::Branch(
                        std::move(compiled_pred), consq_space.deliver(), alter_space.deliver())));
    if (consq_space.terminated() && alter_space.terminated()) {
        return space.terminate(pos);
    }

    if (consequence.isAsync() || alternative.isAsync()) {
        util::sptr<output::NoParamCallback> succession(new output::NoParamCallback);
        if (!consq_space.terminated()) {
            consq_space.addStmt(pos, util::mkptr(
                    new output::Arithmetics(succession->callMe(
                        pos, util::ptrarr<output::Expression const>()))));
        }
        if (!alter_space.terminated()) {
            alter_space.addStmt(pos, util::mkptr(
                    new output::Arithmetics(succession->callMe(
                        pos, util::ptrarr<output::Expression const>()))));
        }
        util::sref<output::Block> sf(succession->bodyFlow());
        space.block()->addFunc(std::move(succession));
        space.setAsyncSpace(pos, std::vector<std::string>(), sf);
    }
}

bool Branch::isAsync() const
{
    return predicate->isAsync() || consequence.isAsync() || alternative.isAsync();
}

void NameDef::compile(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> init_value(init->compile(space));
    if (init->isLiteral(space.sym())) {
        return space.sym()->defConst(pos, name, *init);
    }
    space.sym()->defName(pos, name);
    space.addStmt(pos, util::mkptr(new output::Arithmetics(util::mkptr(new output::Assignment(
                                                pos
                                              , util::mkptr(new output::Reference(pos, name))
                                              , std::move(init_value))))));
}

bool NameDef::isAsync() const
{
    return init->isAsync();
}

void Return::compile(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> ret(space.ret(*ret_val));
    space.addStmt(pos, util::mkptr(new output::Return(std::move(ret))));
    space.terminate(pos);
}

bool Return::isAsync() const
{
    return ret_val->isAsync();
}

void Import::compile(BaseCompilingSpace& space) const
{
    std::for_each(names.begin()
                , names.end()
                , [&](std::string const& name)
                  {
                      space.sym()->imported(pos, name);
                  });
}

void Export::compile(BaseCompilingSpace& space) const
{
    space.sym()->compileRef(pos, export_point[0]);
    util::sptr<output::Expression const> cval(value->compile(space));
    space.addStmt(pos, util::mkptr(new output::Export(export_point, std::move(cval))));
}

bool Export::isAsync() const
{
    return value->isAsync();
}

void AttrSet::compile(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> csp(set_point->compile(space));
    util::sptr<output::Expression const> cval(value->compile(space));
    space.addStmt(pos, util::mkptr(new output::Arithmetics(util::mkptr(new output::Assignment(
                                                pos, std::move(csp), std::move(cval))))));
}

bool AttrSet::isAsync() const
{
    return set_point->isAsync() || value->isAsync();
}

void ExceptionStall::compile(BaseCompilingSpace& space) const
{
    if (!isAsync()) {
        RegularSubCompilingSpace try_space(space);
        try_block.compile(try_space);
        CatcherSpace catch_space(space);
        catch_block.compile(catch_space);
        space.addStmt(pos, util::mkptr(new output::ExceptionStall(
                        try_space.deliver(), catch_space.deliver())));
        if (try_space.terminated() && catch_space.terminated()) {
            space.terminate(pos);
        }
        return;
    }

    util::sptr<output::AsyncCatcher> catch_func(new output::AsyncCatcher);
    CatcherSpace catch_space(space);
    catch_space.setAsyncSpace(pos, std::vector<std::string>(), catch_func->bodyFlow());
    catch_block.compile(catch_space);

    AsyncTrySpace try_space(space, *catch_func);
    try_block.compile(try_space);

    util::sptr<output::Block> staller(new output::Block);
    util::ptrarr<output::Expression const> args;
    args.append(util::mkptr(new output::ExceptionObj(pos)));
    staller->addStmt(util::mkptr(
            new output::Arithmetics(catch_func->callMe(pos, std::move(args)))));

    space.block()->addFunc(std::move(catch_func));
    space.addStmt(pos, util::mkptr(new output::ExceptionStall(
                                    try_space.deliver(), std::move(staller))));
    if (try_space.terminated() && catch_space.terminated()) {
        return space.terminate(pos);
    }

    if (try_block.isAsync() || catch_block.isAsync()) {
        util::sptr<output::NoParamCallback> succession(new output::NoParamCallback);
        if (!try_space.terminated()) {
            try_space.addStmt(pos, util::mkptr(
                    new output::Arithmetics(succession->callMe(
                        pos, util::ptrarr<output::Expression const>()))));
        }
        if (!catch_space.terminated()) {
            catch_space.addStmt(pos, util::mkptr(
                    new output::Arithmetics(succession->callMe(
                        pos, util::ptrarr<output::Expression const>()))));
        }
        util::sref<output::Block> sf(succession->bodyFlow());
        space.block()->addFunc(std::move(succession));
        space.setAsyncSpace(pos, std::vector<std::string>(), sf);
    }
}

bool ExceptionStall::isAsync() const
{
    return try_block.isAsync() || catch_block.isAsync();
}

void Throw::compile(BaseCompilingSpace& space) const
{
    if (exception->isAsync()) {
        return error::asyncNotAllowedInThrow(pos);
    }
    space.addStmt(pos, util::mkptr(new output::Throw(space.raiseMethod(), exception->compile(space))));
    space.terminate(pos);
}
