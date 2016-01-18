#include "samples-test.h"

using namespace test;

TEST_F(SyntaxSampleTest, Empty)
{
    parseSampleOk("test/grammar/empty.fls");

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (BLOCK_END)
    ;
    DataTree::verify();
}
