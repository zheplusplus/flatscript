#ifndef __STEKIN_OUTPUT_STATEMENT_WRITER_H__
#define __STEKIN_OUTPUT_STATEMENT_WRITER_H__

#include <string>

namespace output {

    void kwReturn();
    void returnNothing();

    void kwDeclare(std::string const& name);

    void branchIf();
    void branchElse();

    void blockBegin();
    void blockEnd();
    void endOfStatement();

}

#endif /* __STEKIN_OUTPUT_STATEMENT_WRITER_H__ */
