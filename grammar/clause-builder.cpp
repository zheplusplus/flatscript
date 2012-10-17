#include <flowcheck/global-filter.h>
#include <flowcheck/node-base.h>
#include <flowcheck/function.h>
#include <report/errors.h>

#include "clause-builder.h"
#include "stmt-nodes.h"

using namespace grammar;

namespace {

    struct DummyAcceptor
        : public Acceptor
    {
        DummyAcceptor()
            : Acceptor(misc::position())
        {}

        void acceptStmt(util::sptr<Statement const>) {}
        void acceptFunc(util::sptr<Function const>) {}
        void deliverTo(util::sref<Acceptor>) {}
    };

}

void AcceptorStack::add(int level, util::sptr<Acceptor> acc)
{
    _prepareLevel(level, acc->pos);
    _acceptors.push_back(std::move(acc));
}

void AcceptorStack::nextStmt(int level, util::sptr<Statement const> stmt)
{
    _prepareLevel(level, stmt->pos);
    _acceptors.back()->acceptStmt(std::move(stmt));
}

void AcceptorStack::nextFunc(int level, util::sptr<Function const> func)
{
    _prepareLevel(level, func->pos);
    _acceptors.back()->acceptFunc(std::move(func));
}

void AcceptorStack::matchElse(int level, misc::position const& pos)
{
    _prepareLevel(level + 1, pos);
    _acceptors.back()->acceptElse(pos);
}

void AcceptorStack::_fillTo(int level, misc::position const& pos)
{
    if (int(_acceptors.size()) <= level) {
        error::excessiveIndent(pos);
        while (int(_acceptors.size()) <= level) {
            _acceptors.push_back(std::move(util::mkptr(new DummyAcceptor)));
        }
    }
}

void AcceptorStack::_shrinkTo(int level)
{
    while (level + 1 < int(_acceptors.size())) {
        util::sptr<Acceptor> deliverer(std::move(_acceptors.back()));
        _acceptors.pop_back();
        deliverer->deliverTo(*_acceptors.back());
    }
}

void AcceptorStack::_prepareLevel(int level, misc::position const& pos)
{
    _fillTo(level, pos);
    _shrinkTo(level);
}

Block AcceptorStack::packAll()
{
    _shrinkTo(0);
    return std::move(_packer->pack());
}

AcceptorStack::AcceptorStack()
    : _packer(_prepare1stAcceptor())
{}

util::sref<AcceptorStack::AcceptorOfPack> AcceptorStack::_prepare1stAcceptor()
{
    util::sptr<AcceptorOfPack> packer(new AcceptorOfPack);
    util::sref<AcceptorOfPack> ref = *packer;
    _acceptors.push_back(std::move(packer));
    return ref;
}

void AcceptorStack::AcceptorOfPack::acceptStmt(util::sptr<Statement const> stmt)
{
    _pack.addStmt(std::move(stmt));
}

void AcceptorStack::AcceptorOfPack::acceptFunc(util::sptr<Function const> func)
{
    _pack.addFunc(std::move(func));
}

Block AcceptorStack::AcceptorOfPack::pack()
{
    return std::move(_pack);
}

void ClauseBuilder::addArith(int indent_len, util::sptr<Expression const> arith)
{
    misc::position pos(arith->pos);
    _stack.nextStmt(indent_len, std::move(util::mkptr(new Arithmetics(pos, std::move(arith)))));
}

void ClauseBuilder::addVarDef(int indent_len
                            , std::string const& name
                            , util::sptr<Expression const> init)
{
    misc::position pos(init->pos);
    _stack.nextStmt(indent_len, std::move(util::mkptr(new VarDef(pos, name, std::move(init)))));
}

void ClauseBuilder::addReturn(int indent_len, util::sptr<Expression const> ret_val)
{
    misc::position pos(ret_val->pos);
    _stack.nextStmt(indent_len, std::move(util::mkptr(new Return(pos, std::move(ret_val)))));
}

void ClauseBuilder::addReturnNothing(int indent_len, misc::position const& pos)
{
    _stack.nextStmt(indent_len, std::move(util::mkptr(new ReturnNothing(pos))));
}

void ClauseBuilder::addFunction(int indent_len
                              , misc::position const& pos
                              , std::string const& name
                              , std::vector<std::string> const& params)
{
    _stack.add(indent_len, std::move(util::mkptr(new FunctionAcceptor(pos, name, params))));
}

void ClauseBuilder::addIf(int indent_len, util::sptr<Expression const> condition)
{
    misc::position pos(condition->pos);
    _stack.add(indent_len, std::move(util::mkptr(new IfAcceptor(pos, std::move(condition)))));
}

void ClauseBuilder::addIfnot(int indent_len, util::sptr<Expression const> condition)
{
    misc::position pos(condition->pos);
    _stack.add(indent_len, std::move(util::mkptr(new IfnotAcceptor(pos, std::move(condition)))));
}

void ClauseBuilder::addElse(int indent_len, misc::position const& pos)
{
    _stack.matchElse(indent_len, pos);
}

util::sptr<flchk::Filter> ClauseBuilder::buildAndClear()
{
    return _stack.packAll().compile(util::mkptr(new flchk::GlobalFilter));
}
