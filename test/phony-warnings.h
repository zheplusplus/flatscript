#ifndef __STEKIN_TEST_PHONY_WARNINGS_H__
#define __STEKIN_TEST_PHONY_WARNINGS_H__

#include <vector>

#include <report/warnings.h>

namespace test {

    struct ConsqBranchTerminatedRec {
        explicit ConsqBranchTerminatedRec(misc::position const& ps)
            : pos(ps)
        {}

        misc::position const pos;
    };

    struct AlterBranchTerminatedRec {
        explicit AlterBranchTerminatedRec(misc::position const& ps)
            : pos(ps)
        {}

        misc::position const pos;
    };

    struct BothBranchesTerminatedRec {
        BothBranchesTerminatedRec(misc::position const& consq_ps, misc::position const& alter_ps)
            : consq_pos(consq_ps)
            , alter_pos(alter_ps)
        {}

        misc::position const consq_pos;
        misc::position const alter_pos;
    };

    void clearWarn();

    std::vector<ConsqBranchTerminatedRec> getConsqBranchTerminated();
    std::vector<AlterBranchTerminatedRec> getAlterBranchTerminated();
    std::vector<BothBranchesTerminatedRec> getBothBranchesTerminated();

}

#endif /* __STEKIN_TEST_PHONY_WARNINGS_H__ */
