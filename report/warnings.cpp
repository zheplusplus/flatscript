#include <iostream>

#include "warnings.h"

void warning::oneOrTwoBranchesTerminated(
        util::sref<misc::position const> consq_term_pos_or_nul_if_not_term
      , util::sref<misc::position const> alter_term_pos_or_nul_if_not_term)
{
    std::cerr << "Warning: branch termination panic." << std::endl;
    if (consq_term_pos_or_nul_if_not_term.not_nul()) {
        std::cerr << "    termination at: " << consq_term_pos_or_nul_if_not_term->str()
                  << std::endl;
    }
    if (alter_term_pos_or_nul_if_not_term.not_nul()) {
        std::cerr << "    termination at: " << alter_term_pos_or_nul_if_not_term->str()
                  << std::endl;
    }
}
