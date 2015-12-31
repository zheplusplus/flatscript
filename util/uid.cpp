#include "uid.h"
#include "string.h"

using namespace util;

static uid::id_type global_id(0);

std::string uid::str() const
{
    return util::str(id);
}

bool uid::operator==(uid const& rhs) const
{
    return this->id == rhs.id;
}

bool uid::operator!=(uid const& rhs) const
{
    return !operator==(rhs);
}

uid::uid()
    : id(global_id++)
{}
