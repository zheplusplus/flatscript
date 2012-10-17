#include <list>
#include <gtest/gtest.h>

#include "phony-warnings.h"

using namespace test;

static std::list<ConsqBranchTerminatedRec> consq_branch_terminated_rec;
static std::list<AlterBranchTerminatedRec> alter_branch_terminated_recs;
static std::list<BothBranchesTerminatedRec> both_branches_terminated_recs;

void test::clearWarn()
{
    consq_branch_terminated_rec.clear();
    alter_branch_terminated_recs.clear();
    both_branches_terminated_recs.clear();
}

void warning::oneOrTwoBranchesTerminated(
        util::sref<misc::position const> consq_term_pos_or_nul_if_not_term
      , util::sref<misc::position const> alter_term_pos_or_nul_if_not_term)
{
    if (consq_term_pos_or_nul_if_not_term.nul()) {
        ASSERT_TRUE(alter_term_pos_or_nul_if_not_term.not_nul());
        return alter_branch_terminated_recs.push_back(
                    AlterBranchTerminatedRec(alter_term_pos_or_nul_if_not_term.cp()));
    }
    if (alter_term_pos_or_nul_if_not_term.nul()) {
        ASSERT_TRUE(consq_term_pos_or_nul_if_not_term.not_nul());
        return consq_branch_terminated_rec.push_back(
                ConsqBranchTerminatedRec(consq_term_pos_or_nul_if_not_term.cp()));
    }
    both_branches_terminated_recs.push_back(
               BothBranchesTerminatedRec(consq_term_pos_or_nul_if_not_term.cp()
                                       , alter_term_pos_or_nul_if_not_term.cp()));
}

std::vector<ConsqBranchTerminatedRec> test::getConsqBranchTerminated()
{
    return std::vector<ConsqBranchTerminatedRec>(consq_branch_terminated_rec.begin()
                                               , consq_branch_terminated_rec.end());
}

std::vector<AlterBranchTerminatedRec> test::getAlterBranchTerminated()
{
    return std::vector<AlterBranchTerminatedRec>(alter_branch_terminated_recs.begin()
                                               , alter_branch_terminated_recs.end());
}

std::vector<BothBranchesTerminatedRec> test::getBothBranchesTerminated()
{
    return std::vector<BothBranchesTerminatedRec>(both_branches_terminated_recs.begin()
                                                , both_branches_terminated_recs.end());
}
