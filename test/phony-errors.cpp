#include <list>

#include "phony-errors.h"

using namespace test;

static std::list<TabAsIndRec> tab_as_ind_recs;
static std::list<BadIndentRec> bad_indent_recs;
static std::list<InvCharRec> inv_char_recs;

static std::list<ElseNotMatchRec> else_not_matches_recs;
static std::list<IfMatchedRec> if_matched_recs;
static std::list<ExcessIndRec> excess_ind_recs;
static std::list<FlowTerminatedRec> flow_terminated_recs;

static std::list<FuncForbiddenRec> forbidden_func_recs;
static std::list<ForbidDefRec> forbid_var_def_recs;
static std::list<VarRedefRec> local_redefs;
static std::list<InvalidRefRec> invalid_refs;

static std::list<RetTypeConflictRec> ret_type_conflict_recs;
static std::list<RetTypeUnresolvableRec> ret_type_unresolvable_recs;
static std::list<CondNotBoolRec> cond_not_bool_recs;
static std::list<VarNondefRec> var_nondefs;
static std::list<NABinaryOpRec> na_binary_ops;
static std::list<NAPreUnaryOpRec> na_pre_unary_ops;

static std::list<VariableNotCallableRec> variable_not_callables;
static std::list<VarCallArgCountWrong> var_call_arg_count_wrong_recs;
static std::list<ListMemberTypesNotSame> list_member_types_not_same_recs;
static std::list<MemberCallNotFound> member_call_not_found_recs;

static std::list<PipeRefNotInListContext> pipe_ref_not_in_list_context;
static std::list<PipeNotApplyOnList> pipe_not_apply_on_list;

static bool has_err = false;

void test::clearErr()
{
    has_err = false;

    tab_as_ind_recs.clear();
    bad_indent_recs.clear();
    inv_char_recs.clear();

    else_not_matches_recs.clear();
    if_matched_recs.clear();
    excess_ind_recs.clear();
    flow_terminated_recs.clear();

    forbidden_func_recs.clear();
    forbid_var_def_recs.clear();
    local_redefs.clear();
    invalid_refs.clear();

    var_nondefs.clear();
    na_binary_ops.clear();
    na_pre_unary_ops.clear();
    ret_type_conflict_recs.clear();
    ret_type_unresolvable_recs.clear();
    cond_not_bool_recs.clear();

    variable_not_callables.clear();
    var_call_arg_count_wrong_recs.clear();
    list_member_types_not_same_recs.clear();
    member_call_not_found_recs.clear();

    pipe_ref_not_in_list_context.clear();
    pipe_not_apply_on_list.clear();
}

void yyerror(std::string const&)
{
    has_err = true;
}

bool error::hasError()
{
    return has_err;
}

void error::tabAsIndent(misc::position const& pos)
{
    has_err = true;
    tab_as_ind_recs.push_back(TabAsIndRec(pos));
}

void error::badIndent(misc::position const& pos)
{
    has_err = true;
    bad_indent_recs.push_back(BadIndentRec(pos));
}

void error::invalidChar(misc::position const& pos, int character)
{
    has_err = true;
    inv_char_recs.push_back(InvCharRec(pos, character));
}

void error::elseNotMatchIf(misc::position const& pos)
{
    has_err = true;
    else_not_matches_recs.push_back(ElseNotMatchRec(pos));
}

void error::ifAlreadyMatchElse(misc::position const& prev_else_pos
                             , misc::position const& this_else_pos)
{
    has_err = true;
    if_matched_recs.push_back(IfMatchedRec(prev_else_pos, this_else_pos));
}

void error::excessiveIndent(misc::position const& pos)
{
    has_err = true;
    excess_ind_recs.push_back(ExcessIndRec(pos));
}

void error::flowTerminated(misc::position const& this_pos, misc::position const& prev_pos)
{
    has_err = true;
    flow_terminated_recs.push_back(FlowTerminatedRec(this_pos, prev_pos));
}

void error::forbidDefFunc(misc::position const& pos, std::string const& name)
{
    has_err = true;
    forbidden_func_recs.push_back(FuncForbiddenRec(pos, name));
}

void error::forbidDefVar(misc::position const& pos, std::string const& name)
{
    has_err = true;
    forbid_var_def_recs.push_back(ForbidDefRec(pos, name));
}

void error::varAlreadyInLocal(misc::position const& prev_def_pos
                            , misc::position const& this_def_pos
                            , std::string const& name)
{
    has_err = true;
    local_redefs.push_back(VarRedefRec(prev_def_pos, this_def_pos, name));
}

void error::varRefBeforeDef(misc::position const& def_pos
                          , std::list<misc::position> const& ref_positions
                          , std::string const& name)
{
    has_err = true;
    invalid_refs.push_back(InvalidRefRec(ref_positions.begin()
                                       , ref_positions.end()
                                       , def_pos
                                       , name));
}

void error::varNotDef(misc::position const& ref_pos, std::string const& name)
{
    has_err = true;
    var_nondefs.push_back(VarNondefRec(ref_pos, name));
}

void error::binaryOpNotAvai(misc::position const& pos
                          , std::string const& op_img
                          , std::string const& lhst
                          , std::string const& rhst)
{
    has_err = true;
    na_binary_ops.push_back(NABinaryOpRec(pos, op_img, lhst, rhst));
}

void error::preUnaryOpNotAvai(misc::position const& pos
                            , std::string const& op_img
                            , std::string const& rhst)
{
    has_err = true;
    na_pre_unary_ops.push_back(NAPreUnaryOpRec(pos, op_img, rhst));
}

void error::conflictReturnType(std::string const& prev_type_name
                             , std::string const& this_type_name
                             , misc::trace const& trace)
{
    has_err = true;
    ret_type_conflict_recs.push_back(RetTypeConflictRec(prev_type_name, this_type_name, trace));
}

void error::returnTypeUnresolvable(std::string const& name, int arg_count, misc::trace const& trace)
{
    has_err = true;
    ret_type_unresolvable_recs.push_back(RetTypeUnresolvableRec(name, arg_count, trace));
}

void error::condNotBool(misc::position const& pos, std::string const& actual_type)
{
    has_err = true;
    cond_not_bool_recs.push_back(CondNotBoolRec(pos, actual_type));
}

void error::requestVariableNotCallable(misc::position const& call_pos)
{
    has_err = true;
    variable_not_callables.push_back(VariableNotCallableRec(call_pos));
}

void error::callVariableArgCountWrong(misc::position const& call_pos, int actual, int wanted)
{
    has_err = true;
    var_call_arg_count_wrong_recs.push_back(VarCallArgCountWrong(call_pos, actual, wanted));
}

void error::listMemberTypesNotSame(misc::position const& call_pos)
{
    has_err = true;
    list_member_types_not_same_recs.push_back(ListMemberTypesNotSame(call_pos));
}

void error::memberCallNotFound(misc::position const& pos
                             , std::string const& type_name
                             , std::string const& call_name)
{
    has_err = true;
    member_call_not_found_recs.push_back(MemberCallNotFound(pos, type_name, call_name));
}

void error::pipeReferenceNotInListContext(misc::position const& pos)
{
    has_err = true;
    pipe_ref_not_in_list_context.push_back(PipeRefNotInListContext(pos));
}

void error::pipeNotApplyOnList(misc::position const& pos)
{
    has_err = true;
    pipe_not_apply_on_list.push_back(PipeNotApplyOnList(pos));
}

std::vector<TabAsIndRec> test::getTabAsIndents()
{
    return std::vector<TabAsIndRec>(tab_as_ind_recs.begin(), tab_as_ind_recs.end());
}

std::vector<BadIndentRec> test::getBadIndents()
{
    return std::vector<BadIndentRec>(bad_indent_recs.begin(), bad_indent_recs.end());
}

std::vector<InvCharRec> test::getInvCharRecs()
{
    return std::vector<InvCharRec>(inv_char_recs.begin(), inv_char_recs.end());
}

std::vector<ElseNotMatchRec> test::getElseNotMatches()
{
    return std::vector<ElseNotMatchRec>(else_not_matches_recs.begin(), else_not_matches_recs.end());
}

std::vector<IfMatchedRec> test::getIfMatchedRecs()
{
    return std::vector<IfMatchedRec>(if_matched_recs.begin(), if_matched_recs.end());
}

std::vector<ExcessIndRec> test::getExcessInds()
{
    return std::vector<ExcessIndRec>(excess_ind_recs.begin(), excess_ind_recs.end());
}

std::vector<FlowTerminatedRec> test::getFlowTerminatedRecs()
{
    return std::vector<FlowTerminatedRec>(flow_terminated_recs.begin(), flow_terminated_recs.end());
}

std::vector<FuncForbiddenRec> test::getForbiddenFuncs()
{
    return std::vector<FuncForbiddenRec>(forbidden_func_recs.begin(), forbidden_func_recs.end());
}

std::vector<ForbidDefRec> test::getForbidVarDefs()
{
    return std::vector<ForbidDefRec>(forbid_var_def_recs.begin(), forbid_var_def_recs.end());
}

std::vector<VarRedefRec> test::getLocalRedefs()
{
    return std::vector<VarRedefRec>(local_redefs.begin(), local_redefs.end());
}

std::vector<InvalidRefRec> test::getInvalidRefs()
{
    return std::vector<InvalidRefRec>(invalid_refs.begin(), invalid_refs.end());
}

std::vector<VarNondefRec> test::getNondefs()
{
    return std::vector<VarNondefRec>(var_nondefs.begin(), var_nondefs.end());
}

std::vector<NABinaryOpRec> test::getNABinaryOps()
{
    return std::vector<NABinaryOpRec>(na_binary_ops.begin(), na_binary_ops.end());
}

std::vector<NAPreUnaryOpRec> test::getNAPreUnaryOps()
{
    return std::vector<NAPreUnaryOpRec>(na_pre_unary_ops.begin(), na_pre_unary_ops.end());
}

std::vector<RetTypeConflictRec> test::getRetTypeConflicts()
{
    return std::vector<RetTypeConflictRec>(ret_type_conflict_recs.begin()
                                         , ret_type_conflict_recs.end());
}

std::vector<RetTypeUnresolvableRec> test::getRetTypeUnresolvables()
{
    return std::vector<RetTypeUnresolvableRec>(ret_type_unresolvable_recs.begin()
                                             , ret_type_unresolvable_recs.end());
}

std::vector<CondNotBoolRec> test::getCondNotBools()
{
    return std::vector<CondNotBoolRec>(cond_not_bool_recs.begin(), cond_not_bool_recs.end());
}

std::vector<VariableNotCallableRec> test::getVariableNotCallables()
{
    return std::vector<VariableNotCallableRec>(variable_not_callables.begin()
                                             , variable_not_callables.end());
}

std::vector<VarCallArgCountWrong> test::getVarCallArgCountWrong()
{
    return std::vector<VarCallArgCountWrong>(var_call_arg_count_wrong_recs.begin()
                                           , var_call_arg_count_wrong_recs.end());
}

std::vector<ListMemberTypesNotSame> test::getListMemberTypesNotSame()
{
    return std::vector<ListMemberTypesNotSame>(list_member_types_not_same_recs.begin()
                                             , list_member_types_not_same_recs.end());
}

std::vector<MemberCallNotFound> test::getMemberCallNotFound()
{
    return std::vector<MemberCallNotFound>(member_call_not_found_recs.begin()
                                         , member_call_not_found_recs.end());
}

std::vector<PipeRefNotInListContext> test::getPipeRefNotInListContext()
{
    return std::vector<PipeRefNotInListContext>(pipe_ref_not_in_list_context.begin()
                                              , pipe_ref_not_in_list_context.end());
}

std::vector<PipeNotApplyOnList> test::getPipeNotApplyOnList()
{
    return std::vector<PipeNotApplyOnList>(pipe_not_apply_on_list.begin()
                                         , pipe_not_apply_on_list.end());
}

void error::featureNotSupportWrapListInClosure(misc::position const&) {}
