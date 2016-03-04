#include "uid.h"
#include "string.h"

using namespace util;

static uid::id_type global_id(0);

std::string uid::str() const
{
    return util::str(id);
}

uid::uid()
    : id(global_id++)
{}
