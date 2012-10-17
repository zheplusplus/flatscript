#ifndef __STEKIN_OUTPUT_FUNCTION_WRITER_H__
#define __STEKIN_OUTPUT_FUNCTION_WRITER_H__

#include <vector>
#include <string>

#include <util/pointer.h>

namespace output {

    void writeFuncBegin(std::string const& name, std::vector<std::string> const& params);
    void writeFuncEnd();

    void writeArgsBegin();
    void writeArgsEnd();
    void writeArgSeparator();

    void writeMainBegin();
    void writeMainEnd();

    void pipeMapBegin();
    void pipeMapEnd();
    void pipeFilterBegin();
    void pipeFilterEnd();
    void pipeFinished();

    void pipeElement();
    void pipeIndex();

}

#endif /* __STEKIN_OUTPUT_FUNCTION_WRITER_H__ */
