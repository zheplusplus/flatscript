#include <util/io.h>
#include <grammar/syntax.h>
#include <report/errors.h>

#include "including.h"

using namespace flats;

util::sptr<semantic::Statement const> flats::compileFile(
            std::string const& file, misc::position const& from)
{
    std::string source;
    try {
        if (file.empty()) {
            source = util::read_stdin();
        } else {
            source = util::read_file(file);
        }
    } catch (util::IOError& e) {
        error::externalError(from, e.what());
        throw CompileError();
    }
    util::sptr<semantic::Statement const> global_flow(grammar::build(file, source));
    if (error::hasError()) {
        throw CompileError();
    }
    return std::move(global_flow);
}
