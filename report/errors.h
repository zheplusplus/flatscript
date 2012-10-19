#ifndef __STEKIN_REPORT_ERRORS_H__
#define __STEKIN_REPORT_ERRORS_H__

#include <list>
#include <string>

#include <misc/pos-type.h>

namespace error {

    bool hasError();

    void tabAsIndent(misc::position const& pos);
    void badIndent(misc::position const& pos);
    void invalidChar(misc::position const& pos, int character);

    void modifyName(misc::position const& pos, std::string const& name);
    void sliceStepOmitted(misc::position const& pos);
    void elseNotMatchIf(misc::position const& else_pos);
    void ifAlreadyMatchElse(misc::position const& prev_else_pos
                          , misc::position const& this_else_pos);
    void excessiveIndent(misc::position const& pos);

    void flowTerminated(misc::position const& this_pos, misc::position const& prev_pos);

    void forbidDefFunc(misc::position const& pos, std::string const& name);
    void forbidDefName(misc::position const& pos, std::string const& name);

    void nameAlreadyInLocal(misc::position const& prev_def_pos
                          , misc::position const& this_def_pos
                          , std::string const& name);
    void nameRefBeforeDef(misc::position const& def_pos
                        , std::list<misc::position> const& ref_positions
                        , std::string const& name);

    void nameNotDef(misc::position const& ref_pos, std::string const& name);

    void binaryOpNotAvai(misc::position const& pos
                       , std::string const& op_img
                       , std::string const& lhst_name
                       , std::string const& rhst_name);
    void preUnaryOpNotAvai(misc::position const& pos
                         , std::string const& op_img
                         , std::string const& rhst_name);

    void conflictReturnType(std::string const& prev_ret_type_name
                          , std::string const& this_ret_type_name
                          , misc::trace const& trace);

    void returnTypeUnresolvable(std::string const& name, int arg_count, misc::trace const& trace);

    void condNotBool(misc::position const& pos, std::string const& actual_type);

    void requestNameNotCallable(misc::position const& call_pos);
    void callNameArgCountWrong(misc::position const& call_pos, int actual, int wanted);
    void listMemberTypesNotSame(misc::position const& pos);
    void memberCallNotFound(misc::position const& pos
                          , std::string const& type_name
                          , std::string const& call_name);
    void pipeReferenceNotInListContext(misc::position const& pos);
    void pipeNotApplyOnList(misc::position const& pos);

    void featureNotSupportWrapListInClosure(misc::position const& pos);

}

#endif /* __STEKIN_REPORT_ERRORS_H__ */
