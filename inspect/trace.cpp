#include <iostream>

#include <backtracpp/sig-handler.h>

#include "trace.h"

void inspect::prepare_for_trace()
{
    trac::set_output(std::cerr);
    trac::trace_on_seg_fault();
}
