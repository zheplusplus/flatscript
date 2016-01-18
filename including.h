#ifndef __FLSC_FILE_INCLUDING_H__
#define __FLSC_FILE_INCLUDING_H__

#include <stdexcept>
#include <semantic/node-base.h>
#include <misc/pos-type.h>

namespace flats {

    struct CompileError
        : std::runtime_error
    {
        CompileError()
            : std::runtime_error("")
        {}
    };

    util::sptr<semantic::Statement const> compileFile(
                    std::string const& file, misc::position const& from);

}

#endif /* __FLSC_FILE_INCLUDING_H__ */
