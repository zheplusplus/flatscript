import binascii
import inspect

def lineno():
    return '''\n#line {line} "{file}"\n'''.format(
                line=inspect.currentframe().f_back.f_lineno, file=__file__)

TYPES = ['bool', 'int', 'float', 'string']
def type_mapping(t):
    mapper = {
        'int': 'mpz_class',
        'float': 'mpf_class',
        'string': 'std::string',
    }
    return mapper[t] if t in mapper else t

PRE_UNARY_OPS = ['+', '-', '!', '*']

def pre_unary_op_map(op):
    mapper = {
        '+': '',
    }
    return mapper[op] if op in mapper else op

PRE_UNARY_OPERATIONS = {
    ('!', 'bool'): 'bool',
    ('+', 'int'): 'int',
    ('-', 'int'): 'int',
    ('+', 'float'): 'float',
    ('-', 'float'): 'float',
}

BINARY_OPS = ['+', '-', '*', '/', '%', '=', '!=', '<', '>', '<=', '>=', '||', '&&']
def binary_op_map(op):
    mapper = {
        '=': '==',
    }
    return mapper[op] if op in mapper else op

BINARY_OP_NAMES = {
    '+': 'plus',
    '-': 'minus',
    '*': 'mul',
    '/': 'div',
    '%': 'mod',
    '=': 'eq',
    '!=': 'ne',
    '<': 'lt',
    '>': 'gt',
    '<=': 'le',
    '>=': 'ge',
    '||': 'or',
    '&&': 'and',
}

def gen_binary_operations():
    op = dict()
    for o in ['+', '-', '*', '/']:
        op[(o, 'int', 'int')] = 'int'
        op[(o, 'float', 'float')] = 'float'
        op[(o, 'int', 'float')] = 'float'
        op[(o, 'float', 'int')] = 'float'
    for tp in TYPES:
        op[('+', tp, 'string')] = 'string'
        op[('+', 'string', tp)] = 'string'
    op[('%', 'int', 'int')] = 'int'
    for o in ['=', '!=', '<', '>', '<=', '>=']:
        op[(o, 'int', 'int')] = 'bool'
        op[(o, 'float', 'float')] = 'bool'
        op[(o, 'int', 'float')] = 'bool'
        op[(o, 'float', 'int')] = 'bool'
        op[(o, 'string', 'string')] = 'bool'
    op[('=', 'bool', 'bool')] = 'bool'
    op[('!=', 'bool', 'bool')] = 'bool'
    op[('||', 'bool', 'bool')] = 'bool'
    op[('&&', 'bool', 'bool')] = 'bool'
    return op

BINARY_OPERATIONS = gen_binary_operations()

HEADER_BEGIN = lineno() + '''
#ifndef __STEKIN_SEMANTIC_CONSTANT_FOLD_H__
#define __STEKIN_SEMANTIC_CONSTANT_FOLD_H__

#include "node-base.h"

namespace semantic {

    util::sptr<output::Expression const> compileLiteral(Expression const* e,
                                                        util::sref<SymbolTable const> st);
'''

HEADER_END = lineno() + '''
}

#endif /* __STEKIN_SEMANTIC_CONSTANT_FOLD_H__ */
'''

def build_pre_unary_decl():
    r = lineno() + '''
        std::string foldPreUnaryType(std::string const& op, std::string const& rtype);'''
    fold_funcs = [lineno() + '''
        {tp} foldPreUnary{type_capital}Value(misc::position const& pos, std::string const& op,
                                             util::sptr<Expression const> const& rhs,
                                             util::sref<SymbolTable const> st);
    '''.format(tp=type_mapping(t), type_capital=t.title()) for t in TYPES]
    return r + '\n'.join(fold_funcs)

def build_binary_decl():
    r = lineno() + '''
        std::string foldBinaryType(std::string const& op, std::string const& ltype,
                                   std::string const& rtype);'''
    fold_funcs = [lineno() + '''
        {tp} foldBinary{type_capital}Value(misc::position const& pos, std::string const& op,
                                           util::sptr<Expression const> const& lhs,
                                           util::sptr<Expression const> const& rhs,
                                           util::sref<SymbolTable const> st);
    '''.format(tp=type_mapping(t), type_capital=t.title()) for t in TYPES]
    return r + '\n'.join(fold_funcs)

def write_decl():
    with open('semantic/const-fold.h', 'w') as out:
        out.write(HEADER_BEGIN + build_pre_unary_decl() + build_binary_decl() + HEADER_END)
        out.flush()

IMPL_BEGIN = lineno() + '''
#include <map>

#include <output/expr-nodes.h>
#include <util/string.h>
#include <report/errors.h>

#include "const-fold.h"
#include "filter.h"
#include "expr-nodes.h"

using namespace semantic;

struct PreUnaryKey {
    PreUnaryKey(std::string const& o, std::string const& r)
        : op(o)
        , rtype(r)
    {}

    bool operator==(PreUnaryKey const& rhs) const
    {
        return op == rhs.op && rtype == rhs.rtype;
    }

    bool operator<(PreUnaryKey const& rhs) const
    {
        if (op == rhs.op) {
            return rtype < rhs.rtype;
        }
        return op < rhs.op;
    }

    std::string const op;
    std::string const rtype;
};

struct BinaryKey {
    BinaryKey(std::string const& o, std::string const& l, std::string const& r)
        : op(o)
        , ltype(l)
        , rtype(r)
    {}

    bool operator==(BinaryKey const& rhs) const
    {
        return op == rhs.op && ltype == rhs.ltype && rtype == rhs.rtype;
    }

    bool operator<(BinaryKey const& rhs) const
    {
        if (op != rhs.op) {
            return op < rhs.op;
        }
        if (ltype != rhs.ltype) {
            return ltype < rhs.ltype;
        }
        return rtype < rhs.rtype;
    }

    std::string const op;
    std::string const ltype;
    std::string const rtype;
};

template <typename L>
std::string operator+(L const& lhs, std::string const& rhs)
{
    return util::str(lhs) + rhs;
}

template <typename R>
std::string operator+(std::string const& lhs, R const& rhs)
{
    return lhs + util::str(rhs);
}

std::string operator+(std::string const& lhs, std::string const& rhs)
{
    std::string res(lhs);
    res.append(rhs);
    return res;
}

typedef std::map<std::string, std::map<std::string, std::string>> PreUnaryTypeMap;
static PreUnaryTypeMap genPreUnaryTypeMap();
static PreUnaryTypeMap preUnaryTypeMap(genPreUnaryTypeMap());

std::string semantic::foldPreUnaryType(std::string const& op, std::string const& rtype)
{
    return preUnaryTypeMap[op][rtype];
}

typedef std::map<std::string, std::map<std::string, std::map<std::string, std::string>>>
        BinaryTypeMap;

static BinaryTypeMap genBinaryTypeMap();
static BinaryTypeMap binaryTypeMap(genBinaryTypeMap());

std::string semantic::foldBinaryType(std::string const& op, std::string const& ltype,
                                     std::string const& rtype)
{
    return binaryTypeMap[op][ltype][rtype];
}
'''

def build_compile_literals():
    return lineno() + '''
        util::sptr<output::Expression const> semantic::compileLiteral(
                                Expression const* e, util::sref<SymbolTable const> st)
        $(
            {literal_types}
            return util::sptr<output::Expression const>(nullptr);
        )$
    '''.format(literal_types=''.join([lineno() + '''
        if (e->literalType(st) == "{tp}") $(
            return util::mkptr(new output::{tp_cap}Literal(e->pos, e->{tp}Value(st)));
        )$
    '''.format(tp=tp, tp_cap=tp.title()) for tp in TYPES ]))

def build_op_funcs():
    op_funcs = []
    for op_img, op_name in BINARY_OP_NAMES.iteritems():
        for tp in TYPES:
            op_funcs.append(lineno() + '''
                template <typename L, typename R>
                {rt} {tp}_{op_name}_operate(misc::position const&, L const& lhs, R const& rhs)
                $(
                    return lhs {op_img} rhs;
                )$
            '''.format(op_img=binary_op_map(op_img), op_name=op_name, rt=type_mapping(tp), tp=tp))
    for rhs_type in ['int', 'float']:
        for tp in TYPES:
            op_funcs.append(lineno() + '''
                template <typename L>
                {rt} {tp}_div_operate(misc::position const& pos, L const& lhs, {rtp} const& rhs)
                $(
                    if (rhs == 0) $(
                        error::divZero(pos);
                        return {rt}();
                    )$
                    return lhs / rhs;
                )$

                template <typename L>
                {rt} {tp}_mod_operate(misc::position const& pos, L const& lhs, {rtp} const& rhs)
                $(
                    if (rhs == 0) $(
                        error::divZero(pos);
                        return {rt}();
                    )$
                    return lhs % rhs;
                )$
            '''.format(rt=type_mapping(tp), tp=tp, rtp=type_mapping(rhs_type)))
    return ''.join(op_funcs)

def build_type_map_impl():
    pre_unary_map_gen = lineno() + '''
        static PreUnaryTypeMap genPreUnaryTypeMap() {
            PreUnaryTypeMap map;
    '''
    for o in PRE_UNARY_OPS:
        for rtp in TYPES:
            pre_unary_map_gen += 'map["{o}"]["{rtp}"] = "{tp}";\n'.format(
                o=o, rtp=rtp,
                tp=(PRE_UNARY_OPERATIONS[(o, rtp)] if (o, rtp) in PRE_UNARY_OPERATIONS else 'bool'))
    pre_unary_map_gen += 'return map;}\n'

    binary_map_gen = lineno() + '''
        static BinaryTypeMap genBinaryTypeMap() {
            BinaryTypeMap map;
    '''
    for o in BINARY_OPS:
        for ltp in TYPES:
            for rtp in TYPES:
                binary_map_gen += 'map["{o}"]["{ltp}"]["{rtp}"] = "{tp}";\n'.format(
                        o=o, ltp=ltp, rtp=rtp,
                        tp=(BINARY_OPERATIONS[(o, ltp, rtp)] if (o, ltp, rtp) in BINARY_OPERATIONS
                                                             else 'bool'))
    binary_map_gen += 'return map;}\n'
    return pre_unary_map_gen + binary_map_gen

def build_op_map_impl():
    pre_unary_base_classes = [lineno() + '''
        struct PreUnary{tp}Folder $(
            virtual ~PreUnary{tp}Folder() $()$
            virtual {rt} operate(util::sptr<Expression const> const& rhs,
                                 util::sref<SymbolTable const> st) = 0;
        )$;

        typedef std::map<PreUnaryKey, PreUnary{tp}Folder*> PreUnary{tp}FoldersMap;
        static PreUnary{tp}FoldersMap genPreUnary{tp}FoldersMap();
        static PreUnary{tp}FoldersMap preUnary{tp}FoldersMap(genPreUnary{tp}FoldersMap());

        {rt} semantic::foldPreUnary{tp_cap}Value(misc::position const& pos, std::string const& op_img,
                                              util::sptr<Expression const> const& rhs,
                                              util::sref<SymbolTable const> st)
        $(
            if (preUnary{tp}FoldersMap.find(PreUnaryKey(op_img, rhs->literalType(st)))
                    == preUnary{tp}FoldersMap.end())
            $(
                error::preUnaryOpNotAvai(pos, op_img, rhs->literalType(st));
                return rhs->{tp}Value(st);
            )$
            return preUnary{tp}FoldersMap[PreUnaryKey(op_img, rhs->literalType(st))]
                                            ->operate(rhs, st);
        )$
    '''.format(tp=t, tp_cap=t.title(), rt=type_mapping(t)) for t in TYPES]
    pre_unary_impl_classes_mapper = { tp: [] for tp in TYPES }
    for op_detail, rt in PRE_UNARY_OPERATIONS.iteritems():
        pre_unary_impl_classes_mapper[rt].append(op_detail)
    pre_unary_ops = []
    for tp in TYPES:
        pre_unary_impl_classes = []
        pre_unary_impl_obj_mappings = []
        for op_detail in pre_unary_impl_classes_mapper[tp]:
            pre_unary_impl_classes.append(lineno() + '''
                struct PreUnary_{ophex}_{rtp}_Folder : PreUnary{tp}Folder $(
                    {rt} operate(util::sptr<Expression const> const& rhs,
                                 util::sref<SymbolTable const> st)
                    $(
                        return {mapped_op}rhs->{rtp}Value(st);
                    )$
                )$ preUnary_{ophex}_{rtp}_folder_obj;
            '''.format(tp=tp, rtp=op_detail[1], ophex=binascii.hexlify(op_detail[0]),
                       mapped_op=pre_unary_op_map(op_detail[0]), rt=type_mapping(tp)))
            pre_unary_impl_obj_mappings.append('''
                map[PreUnaryKey("{op}", "{rtp}")] = &preUnary_{ophex}_{rtp}_folder_obj;
            '''.format(op=op_detail[0], rtp=op_detail[1], ophex=binascii.hexlify(op_detail[0])))
        pre_unary_ops.append(lineno() + '''
        {classes}

        PreUnary{tp}FoldersMap genPreUnary{tp}FoldersMap()
        $(
            PreUnary{tp}FoldersMap map;
            {map_ops}
            return map;
        )$
        '''.format(tp=tp, map_ops=''.join(pre_unary_impl_obj_mappings),
                   classes=''.join(pre_unary_impl_classes)))

    binary_base_classes = [lineno() + '''
        struct Binary{tp}Folder $(
            virtual ~Binary{tp}Folder() $()$
            virtual {rt} operate(misc::position const& pos,
                                 util::sptr<Expression const> const& lhs,
                                 util::sptr<Expression const> const& rhs,
                                 util::sref<SymbolTable const> st) = 0;
        )$;

        typedef std::map<BinaryKey, Binary{tp}Folder*> Binary{tp}FoldersMap;
        static Binary{tp}FoldersMap genBinary{tp}FoldersMap();
        static Binary{tp}FoldersMap binary{tp}FoldersMap(genBinary{tp}FoldersMap());

        {rt} semantic::foldBinary{tp_cap}Value(misc::position const& pos, std::string const& op_img,
                                               util::sptr<Expression const> const& lhs,
                                               util::sptr<Expression const> const& rhs,
                                               util::sref<SymbolTable const> st)
        $(
            if (binary{tp}FoldersMap.find(BinaryKey(op_img, lhs->literalType(st),
                                          rhs->literalType(st))) == binary{tp}FoldersMap.end())
            $(
                error::binaryOpNotAvai(pos, op_img, lhs->literalType(st), rhs->literalType(st));
                return rhs->{tp}Value(st);
            )$
            return binary{tp}FoldersMap[BinaryKey(
                  op_img, lhs->literalType(st), rhs->literalType(st))]->operate(pos, lhs, rhs, st);
        )$
    '''.format(tp=t, tp_cap=t.title(), rt=type_mapping(t)) for t in TYPES]
    binary_impl_classes_mapper = { tp: [] for tp in TYPES }
    for op_detail, rt in BINARY_OPERATIONS.iteritems():
        binary_impl_classes_mapper[rt].append(op_detail)
    binary_ops = []
    for tp in TYPES:
        binary_impl_classes = []
        binary_impl_obj_mappings = []
        for op_detail in binary_impl_classes_mapper[tp]:
            binary_impl_classes.append(lineno() + '''
                struct Binary_{ophex}_{ltp}_{rtp}_Folder : Binary{tp}Folder $(
                    {rt} operate(misc::position const& pos,
                                 util::sptr<Expression const> const& lhs,
                                 util::sptr<Expression const> const& rhs,
                                 util::sref<SymbolTable const> st)
                    $(
                        return {tp}_{op_name}_operate(pos, lhs->{ltp}Value(st),
                                                      rhs->{rtp}Value(st));
                    )$
                )$ binary_{ophex}_{ltp}_{rtp}_folder_obj;
            '''.format(ltp=op_detail[1], rtp=op_detail[2], ophex=binascii.hexlify(op_detail[0]),
                       op_name=BINARY_OP_NAMES[op_detail[0]], tp=tp, rt=type_mapping(tp)))
            binary_impl_obj_mappings.append('''
                map[BinaryKey("{op}", "{ltp}", "{rtp}")] = &binary_{ophex}_{ltp}_{rtp}_folder_obj;
            '''.format(op=op_detail[0], ltp=op_detail[1], rtp=op_detail[2],
                       ophex=binascii.hexlify(op_detail[0])))
        binary_ops.append(lineno() + '''
        {classes}

        Binary{tp}FoldersMap genBinary{tp}FoldersMap()
        $(
            Binary{tp}FoldersMap map;
            {map_ops}
            return map;
        )$
        '''.format(tp=tp, map_ops=''.join(binary_impl_obj_mappings),
                   classes=''.join(binary_impl_classes)))

    return (''.join(pre_unary_base_classes) +
            ''.join(pre_unary_ops) +
            ''.join(binary_base_classes) +
            ''.join(binary_ops))

def write_impl():
    with open('semantic/const-fold.cpp', 'w') as out:
        out.write((IMPL_BEGIN + build_compile_literals() + build_op_funcs() + build_type_map_impl()
                              + build_op_map_impl()).replace('$(', '{').replace(')$', '}'))
        out.flush()

write_decl()
write_impl()
