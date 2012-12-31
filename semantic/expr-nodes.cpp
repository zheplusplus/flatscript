#include <algorithm>
#include <map>

#include <output/expr-nodes.h>
#include <util/string.h>
#include <report/errors.h>

#include "expr-nodes.h"
#include "function.h"
#include "symbol-table.h"
#include "filter.h"
#include "const-fold.h"

using namespace semantic;

static std::vector<util::sptr<output::Expression const>> compileList(
                        std::vector<util::sptr<Expression const>> const& list
                      , util::sref<SymbolTable> st)
{
    std::vector<util::sptr<output::Expression const>> compiled_list;
    std::for_each(list.begin()
                , list.end()
                , [&](util::sptr<Expression const> const& value)
                  {
                      compiled_list.push_back(value->compile(st));
                  });
    return std::move(compiled_list);
}

util::sptr<output::Expression const> PreUnaryOp::compile(util::sref<SymbolTable> st) const
{
    if (isLiteral(st)) {
        return compileLiteral(this, st);
    }
    return util::mkptr(new output::PreUnaryOp(pos, op_img, rhs->compile(st)));
}

bool PreUnaryOp::isLiteral(util::sref<SymbolTable const> st) const
{
    return rhs->isLiteral(st);
}

std::string PreUnaryOp::literalType(util::sref<SymbolTable const> st) const
{
    return foldPreUnaryType(op_img, rhs->literalType(st));
}

bool PreUnaryOp::boolValue(util::sref<SymbolTable const> st) const
{
    return foldPreUnaryBoolValue(pos, op_img, rhs, st);
}

mpz_class PreUnaryOp::intValue(util::sref<SymbolTable const> st) const
{
    return foldPreUnaryIntValue(pos, op_img, rhs, st);
}

mpf_class PreUnaryOp::floatValue(util::sref<SymbolTable const> st) const
{
    return foldPreUnaryFloatValue(pos, op_img, rhs, st);
}

std::string PreUnaryOp::stringValue(util::sref<SymbolTable const> st) const
{
    return foldPreUnaryStringValue(pos, op_img, rhs, st);
}

util::sptr<output::Expression const> BinaryOp::compile(util::sref<SymbolTable> st) const
{
    if (isLiteral(st)) {
        return compileLiteral(this, st);
    }
    return util::mkptr(new output::BinaryOp(pos, lhs->compile(st), op_img, rhs->compile(st)));
}

bool BinaryOp::isLiteral(util::sref<SymbolTable const> st) const
{
    return lhs->isLiteral(st) && rhs->isLiteral(st);
}

std::string BinaryOp::literalType(util::sref<SymbolTable const> st) const
{
    return foldBinaryType(op_img, lhs->literalType(st), rhs->literalType(st));
}

bool BinaryOp::boolValue(util::sref<SymbolTable const> st) const
{
    return foldBinaryBoolValue(pos, op_img, lhs, rhs, st);
}

mpz_class BinaryOp::intValue(util::sref<SymbolTable const> st) const
{
    return foldBinaryIntValue(pos, op_img, lhs, rhs, st);
}

mpf_class BinaryOp::floatValue(util::sref<SymbolTable const> st) const
{
    return foldBinaryFloatValue(pos, op_img, lhs, rhs, st);
}

std::string BinaryOp::stringValue(util::sref<SymbolTable const> st) const
{
    return foldBinaryStringValue(pos, op_img, lhs, rhs, st);
}

util::sptr<output::Expression const> TypeOf::compile(util::sref<SymbolTable> st) const
{
    if (isLiteral(st)) {
        return util::mkptr(new output::StringLiteral(pos, stringValue(st)));
    }
    return util::mkptr(new output::PreUnaryOp(pos, "typeof ", expr->compile(st)));
}

bool TypeOf::isLiteral(util::sref<SymbolTable const> st) const
{
    return expr->isLiteral(st);
}

std::string TypeOf::literalType(util::sref<SymbolTable const>) const
{
    return "string";
}

std::string TypeOf::stringValue(util::sref<SymbolTable const> st) const
{
    static std::map<std::string, std::string> map{
        { "int", "number" },
        { "float", "number" },
        { "string", "string" },
        { "bool", "boolean" },
    };
    return map[expr->literalType(st)];
}

util::sptr<output::Expression const> Reference::compile(util::sref<SymbolTable> st) const
{
    return st->compileRef(pos, name);
}

bool Reference::isLiteral(util::sref<SymbolTable const> st) const
{
    return st->literalOrNul(name).not_nul();
}

std::string Reference::literalType(util::sref<SymbolTable const> st) const
{
    return st->literalOrNul(name)->literalType(st);
}

bool Reference::boolValue(util::sref<SymbolTable const> st) const
{
    return st->literalOrNul(name)->boolValue(st);
}

mpz_class Reference::intValue(util::sref<SymbolTable const> st) const
{
    return st->literalOrNul(name)->intValue(st);
}

mpf_class Reference::floatValue(util::sref<SymbolTable const> st) const
{
    return st->literalOrNul(name)->floatValue(st);
}

std::string Reference::stringValue(util::sref<SymbolTable const> st) const
{
    return st->literalOrNul(name)->stringValue(st);
}

util::sptr<output::Expression const> BoolLiteral::compile(util::sref<SymbolTable>) const
{
    return util::mkptr(new output::BoolLiteral(pos, value));
}

bool BoolLiteral::isLiteral(util::sref<SymbolTable const>) const
{
    return true;
}

bool BoolLiteral::boolValue(util::sref<SymbolTable const>) const
{
    return value;
}

std::string BoolLiteral::literalType(util::sref<SymbolTable const>) const
{
    return "bool";
}

util::sptr<output::Expression const> IntLiteral::compile(util::sref<SymbolTable>) const
{
    return util::mkptr(new output::IntLiteral(pos, value));
}

bool IntLiteral::isLiteral(util::sref<SymbolTable const>) const
{
    return true;
}

std::string IntLiteral::literalType(util::sref<SymbolTable const>) const
{
    return "int";
}

mpz_class IntLiteral::intValue(util::sref<SymbolTable const>) const
{
    return value;
}

util::sptr<output::Expression const> FloatLiteral::compile(util::sref<SymbolTable>) const
{
    return util::mkptr(new output::FloatLiteral(pos, value));
}

bool FloatLiteral::isLiteral(util::sref<SymbolTable const>) const
{
    return true;
}

std::string FloatLiteral::literalType(util::sref<SymbolTable const>) const
{
    return "float";
}

mpf_class FloatLiteral::floatValue(util::sref<SymbolTable const>) const
{
    return value;
}

util::sptr<output::Expression const> StringLiteral::compile(util::sref<SymbolTable>) const
{
    return util::mkptr(new output::StringLiteral(pos, value));
}

bool StringLiteral::isLiteral(util::sref<SymbolTable const>) const
{
    return true;
}

std::string StringLiteral::literalType(util::sref<SymbolTable const>) const
{
    return "string";
}

bool StringLiteral::boolValue(util::sref<SymbolTable const>) const
{
    return !value.empty();
}

std::string StringLiteral::stringValue(util::sref<SymbolTable const>) const
{
    return value;
}

util::sptr<output::Expression const> ListLiteral::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new output::ListLiteral(pos, compileList(value, st)));
}

util::sptr<output::Expression const> PipeElement::compile(util::sref<SymbolTable>) const
{
    return util::mkptr(new output::PipeElement(pos));
}

util::sptr<output::Expression const> PipeIndex::compile(util::sref<SymbolTable>) const
{
    return util::mkptr(new output::PipeIndex(pos));
}

util::sptr<output::Expression const> PipeKey::compile(util::sref<SymbolTable>) const
{
    return util::mkptr(new output::PipeKey(pos));
}

util::sptr<output::Expression const> ListAppend::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new output::ListAppend(pos, lhs->compile(st), rhs->compile(st)));
}

util::sptr<output::Expression const> Call::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new output::Call(pos, callee->compile(st), compileList(args, st)));
}

util::sptr<output::Expression const> MemberAccess::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new output::MemberAccess(pos, referee->compile(st), member));
}

util::sptr<output::Expression const> Lookup::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new output::Lookup(pos, collection->compile(st), key->compile(st)));
}

util::sptr<output::Expression const> ListSlice::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new output::ListSlice(
                pos, list->compile(st), begin->compile(st), end->compile(st), step->compile(st)));
}

util::sptr<output::Expression const> ListSlice::Default::compile(util::sref<SymbolTable>) const
{
    return util::mkptr(new output::ListSlice::Default(pos));
}

util::sptr<output::Expression const> Dictionary::compile(util::sref<SymbolTable> st) const
{
    std::vector<output::Dictionary::ItemType> compiled_items;
    std::for_each(items.begin()
                , items.end()
                , [&](ItemType const& item)
                  {
                      compiled_items.push_back(std::make_pair(item.first->compile(st)
                                                            , item.second->compile(st)));
                  });
    return util::mkptr(new output::Dictionary(pos, std::move(compiled_items)));
}

util::sptr<output::Expression const> Lambda::compile(util::sref<SymbolTable> st) const
{
    SymbolTable body_st(pos, st, param_names);
    return util::mkptr(new output::Lambda(pos, param_names, body->compile(util::mkref(body_st))));
}
