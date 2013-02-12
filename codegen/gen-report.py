import inspect

POS_TYPE = 'misc::position const&'
INT_TYPE = 'int'
STR_TYPE = 'std::string const&'
POS_LIST_TYPE = 'std::vector<misc::position> const&'

MAP_FIELD_TYPENAME = {
    POS_TYPE : 'misc::position const ',
    INT_TYPE : 'int const ',
    STR_TYPE : 'std::string const ',
    POS_LIST_TYPE: 'std::vector<misc::position> const ',
}

ERROR_VAR = 'has_error'
MARK_ERROR = ERROR_VAR + '=true;'

def lineno():
    return '''\n#line {line} "{file}"\n'''.format(
                line=inspect.currentframe().f_back.f_lineno, file=__file__)

class Param:
    def __init__(self, tp, name):
        self.tp = tp
        self.name = name

    def build(self):
        return self.tp + ' ' + self.name

class ReportFunc:
    def __init__(self, name, implements, *params):
        self.name = name
        self.implements = implements
        self.params = params

    def rec_name(self):
        return self.name[0].upper() + self.name[1:]

    def class_name(self):
        return self.rec_name() + 'Rec'

    def storage_name(self):
        return self.name + 'Recs'

    def build_decl(self):
        return 'void {name}({params});'.format(
                    name=self.name, params=','.join([ p.build() for p in self.params ]))

    def build_impl(self):
        return ('void error::{name}({params})'.format(
                    name=self.name, params=','.join([ p.build() for p in self.params ])) +
                '{' + MARK_ERROR + self.implements + '}')

    def build_test_decl(self):
        return ('''struct ''' + self.class_name() + ''' {\n''' +
                '''{explicit}{class_name}({params}){colon}{init_list}\n'''.format(
                    explicit=('explicit ' if len(self.params) == 1 else ''),
                    class_name=self.class_name(),
                    params=','.join([ p.build() for p in self.params ]),
                    colon=(':' if len(self.params) > 0 else ''),
                    init_list=','.join([ (p.name + '(' + p.name + ')') for p in self.params ])) +
                '''{}\n''' +
                '\n'.join([ (MAP_FIELD_TYPENAME[p.tp] + p.name + ';') for p in self.params ]) +
                '''};\n''' +
                '''std::vector<{class_name}> get{class_name}s();'''.format(
                                                                class_name=self.class_name()))

    def build_test_clearer(self):
        return self.storage_name() + '.clear();'

    def build_test_impl(self):
        return ('std::list<{class_name}> {storage_name};\n'.format(
                    class_name=self.class_name(), storage_name=self.storage_name()) +
                'void error::{name}({params})\n'.format(
                    name=self.name, params=','.join([ p.build() for p in self.params ])) +
                '{' + MARK_ERROR +
                '\n{storage_name}.push_back({class_name}({arguments}));\n'.format(
                    storage_name=self.storage_name(),
                    class_name=self.class_name(),
                    arguments=','.join([ p.name for p in self.params ])) +
                '}\n' +
                'std::vector<{class_name}> test::get{class_name}s()\n'
                        .format(class_name=self.class_name()) + '{' +
                'return std::vector<{class_name}>({storage_name}.begin(), {storage_name}.end());'
                        .format(class_name=self.class_name(), storage_name=self.storage_name()) +
                '}\n')

ERROR_REPORTS = [

ReportFunc(
'tabAsIndent',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    use tab as indent is forbidden." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'invalidChar',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    invalid character " << char(character)
          << " (decimal value: " << character << ")." << std::endl;
'''
, Param(POS_TYPE, 'pos'), Param(INT_TYPE, 'character')),

ReportFunc(
'reservedWord',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    use reserved word: " << token << std::endl;
'''
, Param(POS_TYPE, 'pos'), Param(STR_TYPE, 'token')),

ReportFunc(
'exportToIdent',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    export to identifier is invalid: " << ident << std::endl;
'''
, Param(POS_TYPE, 'pos'), Param(STR_TYPE, 'ident')),

ReportFunc(
'unexpectedToken',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    unexpected " << image << std::endl;
'''
, Param(POS_TYPE, 'pos'), Param(STR_TYPE, 'image')),

ReportFunc(
'emptyLookupKey',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    an expression is supposed as lookup key." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'invalidEmptyExpr',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    expression omitted." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'excessiveExpr',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    more than one expressions in parentheses." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'tooManySliceParts',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    more than 3 expressions as list slice." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'invalidName',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    invalid name; an identifier is supposed here." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'invalidLeftValue',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    invalid left value." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'sliceStepOmitted',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    slice step omitted." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'elseNotMatchIf',
lineno() + '''
std::cerr << else_pos.str() << std::endl;
std::cerr << "    `else' does not match an `if'." << std::endl;
'''
, Param(POS_TYPE, 'else_pos')),

ReportFunc(
'ifAlreadyMatchElse',
lineno() + '''
std::cerr << this_else_pos.str() << std::endl;
std::cerr << "    another `else' already matches the `if' at " << prev_else_pos.str() << std::endl;
'''
, Param(POS_TYPE, 'prev_else_pos'), Param(POS_TYPE, 'this_else_pos')),

ReportFunc(
'incompleteConditional',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    incomplete conditional expression, `else' is expected" << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'invalidIndent',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    invalid indentation" << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'unexpectedEof',
lineno() + '''
std::cerr << "Unexpected end of file; expression not finished" << std::endl;
'''
),

ReportFunc(
'asyncPlaceholderNotArgument',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    asynchronous placeholder should appear as an arugment." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'asyncParamNotExpr',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    asynchronous parameter should appear as an expression." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'moreThanOneAsyncPlaceholder',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    more than one asynchronous placeholders in one call." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'divZero',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    divided by zero." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'flowTerminated',
lineno() + '''
std::cerr << this_pos.str() << std::endl;
std::cerr << "    flow already terminated at " << prev_pos.str() << std::endl;
'''
, Param(POS_TYPE, 'this_pos'), Param(POS_TYPE, 'prev_pos')),

ReportFunc(
'forbidDefFunc',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    " << "attempt define Function `" << name << "' but forbidden here." << std::endl;
'''
, Param(POS_TYPE, 'pos'), Param(STR_TYPE, 'name')),

ReportFunc(
'forbidDefName',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    " << "attempt define name `" << name << "' but forbidden here." << std::endl;
'''
, Param(POS_TYPE, 'pos'), Param(STR_TYPE, 'name')),

ReportFunc(
'nameAlreadyInLocal',
lineno() + '''
std::cerr << this_def_pos.str() << std::endl;
std::cerr << "    name `" << name << "' already defined." << std::endl;
std::cerr << "    see previous definition in local at " << prev_def_pos.str() << std::endl;
'''
, Param(POS_TYPE, 'prev_def_pos'), Param(POS_TYPE, 'this_def_pos'), Param(STR_TYPE, 'name')),

ReportFunc(
'nameRefBeforeDef',
lineno() + '''
std::cerr << def_pos.str() << std::endl;
std::cerr << "    name `" << name << "' definition after reference. see references at:"
          << std::endl;
std::for_each(ref_positions.begin()
            , ref_positions.end()
            , [&](misc::position const& pos)
              {
                  std::cerr << "    - " << pos.str() << std::endl;
              });
'''
, Param(POS_TYPE, 'def_pos'), Param(POS_LIST_TYPE, 'ref_positions'), Param(STR_TYPE, 'name')),

ReportFunc(
'nameNotDef',
lineno() + '''
std::cerr << ref_pos.str() << std::endl;
std::cerr << "    name `" << name << "' not defined." << std::endl;
'''
, Param(POS_TYPE, 'ref_pos'), Param(STR_TYPE, 'name')),

ReportFunc(
'binaryOpNotAvai',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    no available binary operation " << op_img << " for type `" << lhst_name
          << "' and `" << rhst_name << "'." << std::endl;
'''
, Param(POS_TYPE, 'pos'), Param(STR_TYPE, 'op_img'), Param(STR_TYPE, 'lhst_name'),
  Param(STR_TYPE, 'rhst_name')),

ReportFunc(
'preUnaryOpNotAvai',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    no available prefix unary operation " << op_img << " for type `" << rhst_name
          << "'." << std::endl;
'''
, Param(POS_TYPE, 'pos'), Param(STR_TYPE, 'op_img'), Param(STR_TYPE, 'rhst_name')),

ReportFunc(
'condNotBool',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    condition type is not boolean, actual type: " << actual_type << std::endl;
'''
, Param(POS_TYPE, 'pos'), Param(STR_TYPE, 'actual_type')),

ReportFunc(
'returnNotAllowedInPipe',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    return statement not allowed in pipeline." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'pipeReferenceNotInListContext',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    pipeline reference not in list context." << std::endl;
'''
, Param(POS_TYPE, 'pos')),

ReportFunc(
'invalidPropertyName',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    invalid property name: " << expr << std::endl;
std::cerr << "    the expression could not be folded." << std::endl;
'''
, Param(POS_TYPE, 'pos'), Param(STR_TYPE, 'expr')),

ReportFunc(
'importReservedWord',
lineno() + '''
std::cerr << pos.str() << std::endl;
std::cerr << "    import reserved word as name: " << name << std::endl;
'''
, Param(POS_TYPE, 'pos'), Param(STR_TYPE, 'name')),

]

def write_errors_header():
    with open('report/errors.h', 'w') as out:
        out.write(lineno() + '''
            #ifndef __STEKIN_REPORT_ERRORS_H__
            #define __STEKIN_REPORT_ERRORS_H__

            #include <vector>
            #include <string>

            #include <misc/pos-type.h>

            namespace error {
                bool hasError();''' +
                '\n'.join([ r.build_decl() for r in ERROR_REPORTS ]) +
            '''}

            #endif /* __STEKIN_REPORT_ERRORS_H__ */''')
        out.flush()

def write_errors_impl():
    with open('report/errors.cpp', 'w') as out:
        out.write(lineno() + '''
            #include <iostream>
            #include <algorithm>

            #include "errors.h"

            static bool has_error = false;

            namespace grammar { extern int lineno; }

            bool error::hasError()
            {
                return has_error;
            }

            void yyerror(std::string const& msg)
            {
                has_error = true;
                std::cerr << "Line " << grammar::lineno << ":" << std::endl;
                std::cerr << "    " <<  msg << std::endl;
            }''' +
            '\n'.join([ r.build_impl() for r in ERROR_REPORTS ]))
        out.flush()

def write_errors_test_header():
    with open('test/phony-errors.h', 'w') as out:
        out.write('''
            #ifndef __STEKIN_TEST_PHONY_ERRORS_H__
            #define __STEKIN_TEST_PHONY_ERRORS_H__

            #include <vector>

            #include <report/errors.h>

            namespace test {
                void clearErr();
            ''' +
            ''.join([ r.build_test_decl() for r in ERROR_REPORTS ]) +
            '''}

            #endif /* __STEKIN_TEST_PHONY_ERRORS_H__ */''')
        out.flush()

def write_errors_test_impl():
    with open('test/phony-errors.cpp', 'w') as out:
        out.write('''
            #include <list>

            #include "phony-errors.h"

            using namespace test;

            static bool has_error = false;

            void yyerror(std::string const&)
            {
                has_error = true;
            }

            bool error::hasError()
            {
                return has_error;
            }''' +
            '\n'.join([ r.build_test_impl() for r in ERROR_REPORTS ]) +
            '''void test::clearErr()
            {
                has_error = false;''' +
            '\n'.join([ r.build_test_clearer() for r in ERROR_REPORTS ]) +
            '''}''')
        out.flush()

write_errors_header()
write_errors_impl()
write_errors_test_header()
write_errors_test_impl()
