#ifndef __STEKIN_OUTPUT_EXPRESSION_WRITER_H__
#define __STEKIN_OUTPUT_EXPRESSION_WRITER_H__

#include <string>
#include <gmpxx.h>

#include <misc/platform.h>

namespace output {

    void writeInt(mpz_class const& i);
    void writeFloat(mpf_class const& d);
    void writeBool(bool b);
    void writeString(char const* value, int length);

    void reference(std::string const& name);
    void referenceFunc(std::string const& name, int param_count);
    void writeOperator(std::string const& op_img);

    void emptyList();
    void listBegin();
    void listNextMember();
    void listEnd();

    void callBegin();
    void callEnd();

    void listAppendBegin();
    void listAppendEnd();

    void beginExpr();
    void endExpr();

}

#endif /* __STEKIN_OUTPUT_EXPRESSION_WRITER_H__ */
