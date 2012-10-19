#ifndef __STEKIN_TEST_PHONY_ERRORS_H__
#define __STEKIN_TEST_PHONY_ERRORS_H__

#include <vector>

#include <report/errors.h>

namespace test {

    struct TabAsIndRec {
        explicit TabAsIndRec(misc::position const& ps)
            : pos(ps)
        {}

        misc::position const pos;
    };

    struct BadIndentRec {
        explicit BadIndentRec(misc::position const& ps)
            : pos(ps)
        {}

        misc::position const pos;
    };

    struct InvCharRec {
        InvCharRec(misc::position const& ps, int ch)
            : pos(ps)
            , character(ch)
        {}

        misc::position const pos;
        int const character;
    };

    struct ModifyNameRec {
        ModifyNameRec(misc::position const& ps, std::string const& n)
            : pos(ps)
            , name(n)
        {}

        misc::position const pos;
        std::string const name;
    };

    struct SliceStepOmitRec {
        explicit SliceStepOmitRec(misc::position const& ps)
            : pos(ps)
        {}

        misc::position const pos;
    };

    struct ElseNotMatchRec {
        misc::position const pos;

        explicit ElseNotMatchRec(misc::position const& ps)
            : pos(ps)
        {}
    };

    struct IfMatchedRec {
        misc::position const prev_pos;
        misc::position const this_pos;

        IfMatchedRec(misc::position const& ppos, misc::position const& tpos)
            : prev_pos(ppos)
            , this_pos(tpos)
        {}
    };

    struct ExcessIndRec {
        misc::position const pos;

        explicit ExcessIndRec(misc::position const& ps)
            : pos(ps)
        {}
    };

    struct FlowTerminatedRec {
        misc::position const this_pos;
        misc::position const prev_pos;

        FlowTerminatedRec(misc::position const tpos, misc::position const& ppos)
            : this_pos(tpos)
            , prev_pos(ppos)
        {}
    };

    struct FuncForbiddenRec {
        misc::position const pos;
        std::string const name;

        FuncForbiddenRec(misc::position const& ps, std::string const& n)
            : pos(ps)
            , name(n)
        {}
    };

    struct NameRedefRec {
        misc::position const prev_pos;
        misc::position const this_pos;
        std::string const name;

        NameRedefRec(misc::position const& ppos, misc::position const& tpos, std::string const& n)
            : prev_pos(ppos)
            , this_pos(tpos)
            , name(n)
        {}
    };

    struct InvalidRefRec {
        std::vector<misc::position> const ref_positions;
        misc::position const def_pos;
        std::string const name;

        template <typename _LineNoIterator>
        InvalidRefRec(_LineNoIterator pos_begin
                    , _LineNoIterator pos_end
                    , misc::position def_ps
                    , std::string const& n)
            : ref_positions(pos_begin, pos_end)
            , def_pos(def_ps)
            , name(n)
        {}
    };

    struct ForbidDefRec {
        misc::position const pos;
        std::string const name;

        ForbidDefRec(misc::position ps, std::string const& n)
            : pos(ps)
            , name(n)
        {}
    };

    struct NameNondefRec {
        misc::position const ref_pos;
        std::string const name;

        NameNondefRec(misc::position const& rpos, std::string const& n)
            : ref_pos(rpos)
            , name(n)
        {}
    };

    struct NABinaryOpRec {
        misc::position const pos;
        std::string const op_img;
        std::string const lhst_name;
        std::string const rhst_name;

        NABinaryOpRec(misc::position const& ps
                    , std::string const& op
                    , std::string const& lhst
                    , std::string const& rhst)
            : pos(ps)
            , op_img(op)
            , lhst_name(lhst)
            , rhst_name(rhst)
        {}
    };

    struct NAPreUnaryOpRec {
        misc::position const pos;
        std::string const op_img;
        std::string const rhst_name;

        NAPreUnaryOpRec(misc::position const& ps, std::string const& op, std::string const& rhst)
            : pos(ps)
            , op_img(op)
            , rhst_name(rhst)
        {}
    };

    struct RetTypeConflictRec {
        std::string const prev_type_name;
        std::string const this_type_name;
        misc::trace const trace;

        RetTypeConflictRec(std::string const& prev_ret_type_name
                         , std::string const& this_ret_type_name
                         , misc::trace const& t)
            : prev_type_name(prev_ret_type_name)
            , this_type_name(this_ret_type_name)
            , trace(t)
        {}
    };

    struct RetTypeUnresolvableRec {
        std::string const name;
        int const arg_count;
        misc::trace const trace;

        RetTypeUnresolvableRec(std::string const& n, int ac, misc::trace const& t)
            : name(n)
            , arg_count(ac)
            , trace(t)
        {}
    };

    struct CondNotBoolRec {
        misc::position const pos;
        std::string const type_name;

        CondNotBoolRec(misc::position const& ps, std::string const& tname)
            : pos(ps)
            , type_name(tname)
        {}
    };

    struct NameNotCallableRec {
        misc::position const call_pos;

        explicit NameNotCallableRec(misc::position const& cp)
            : call_pos(cp)
        {}
    };

    struct NameCallArgCountWrong {
        misc::position const call_pos;
        int const actual;
        int const wanted;

        NameCallArgCountWrong(misc::position const& c, int a, int w)
            : call_pos(c)
            , actual(a)
            , wanted(w)
        {}
    };

    struct ListMemberTypesNotSame {
        misc::position const pos;

        explicit ListMemberTypesNotSame(misc::position const& ps)
            : pos(ps)
        {}
    };

    struct MemberCallNotFound {
        misc::position const pos;
        std::string const type_name;
        std::string const call_name;

        MemberCallNotFound(misc::position const& ps, std::string const& tn, std::string const& cn)
            : pos(ps)
            , type_name(tn)
            , call_name(cn)
        {}
    };

    struct PipeRefNotInListContext {
        misc::position const pos;

        explicit PipeRefNotInListContext(misc::position const& ps)
            : pos(ps)
        {}
    };

    struct PipeNotApplyOnList {
        misc::position const pos;

        explicit PipeNotApplyOnList(misc::position const& ps)
            : pos(ps)
        {}
    };

    void clearErr();

    std::vector<TabAsIndRec> getTabAsIndents();
    std::vector<BadIndentRec> getBadIndents();
    std::vector<InvCharRec> getInvCharRecs();
    std::vector<ModifyNameRec> getModifyNameRecs();
    std::vector<SliceStepOmitRec> getSliceStepOmits();
    std::vector<ElseNotMatchRec> getElseNotMatches();
    std::vector<IfMatchedRec> getIfMatchedRecs();
    std::vector<ExcessIndRec> getExcessInds();
    std::vector<FlowTerminatedRec> getFlowTerminatedRecs();
    std::vector<FuncForbiddenRec> getForbiddenFuncs();
    std::vector<ForbidDefRec> getForbidNameDefs();
    std::vector<NameRedefRec> getLocalRedefs();
    std::vector<InvalidRefRec> getInvalidRefs();
    std::vector<NameNondefRec> getNondefs();
    std::vector<NABinaryOpRec> getNABinaryOps();
    std::vector<NAPreUnaryOpRec> getNAPreUnaryOps();
    std::vector<RetTypeConflictRec> getRetTypeConflicts();
    std::vector<RetTypeUnresolvableRec> getRetTypeUnresolvables();
    std::vector<CondNotBoolRec> getCondNotBools();
    std::vector<NameNotCallableRec> getNameNotCallables();
    std::vector<NameCallArgCountWrong> getNameCallArgCountWrong();
    std::vector<ListMemberTypesNotSame> getListMemberTypesNotSame();
    std::vector<MemberCallNotFound> getMemberCallNotFound();
    std::vector<PipeRefNotInListContext> getPipeRefNotInListContext();
    std::vector<PipeNotApplyOnList> getPipeNotApplyOnList();

}

#endif /* __STEKIN_TEST_PHONY_ERRORS_H__ */
