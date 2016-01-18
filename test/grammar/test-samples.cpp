#include "samples-test.h"

using namespace test;

TEST_F(SyntaxSampleTest, InvalidCharacters)
{
    parseSampleErr("samples/errors/inv-chars.fls");
    std::vector<InvalidCharRec> recs = getInvalidCharRecs();
    ASSERT_EQ(3, recs.size());
    EXPECT_EQ(misc::position(1), recs[0].pos);
    EXPECT_EQ(int('`'), recs[0].character);
    EXPECT_EQ(misc::position(2), recs[1].pos);
    EXPECT_EQ(int('?'), recs[1].character);
    EXPECT_EQ(misc::position(4), recs[2].pos);
    EXPECT_EQ(int(';'), recs[2].character);
}

TEST_F(SyntaxSampleTest, ErrTabAsIndent)
{
    parseSampleErr("samples/errors/tab-as-ind.fls");
    std::vector<TabAsIndentRec> recs = getTabAsIndentRecs();
    ASSERT_EQ(3, recs.size());
    EXPECT_EQ(misc::position(2), recs[0].pos);
    EXPECT_EQ(misc::position(4), recs[1].pos);
    EXPECT_EQ(misc::position(7), recs[2].pos);
}

TEST_F(SyntaxSampleTest, BadIndentation)
{
    parseSampleErr("samples/errors/bad-indent.fls");
    std::vector<InvalidIndentRec> recs(getInvalidIndentRecs());
    ASSERT_EQ(4, recs.size());
    EXPECT_EQ(misc::position(3), recs[0].pos);
    EXPECT_EQ(misc::position(5), recs[1].pos);
    EXPECT_EQ(misc::position(6), recs[2].pos);
    EXPECT_EQ(misc::position(8), recs[3].pos);
}
