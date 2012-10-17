#ifndef __STEKIN_REPORT_WARNINGS_H__
#define __STEKIN_REPORT_WARNINGS_H__

#include <util/pointer.h>
#include <misc/pos-type.h>

namespace warning {

    void oneOrTwoBranchesTerminated(
                            util::sref<misc::position const> consq_term_pos_or_nul_if_not_term
                          , util::sref<misc::position const> alter_term_pos_or_nul_if_not_term);

}

#endif /* __STEKIN_REPORT_WARNINGS_H__ */
