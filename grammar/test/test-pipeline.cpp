#include "test-clauses.h"

using namespace test;

typedef ClausesTest PipelineTest;

TEST_F(PipelineTest, MultiLinesPipeline)
{
    misc::position pos(1);
    misc::position pos_a(100);
    misc::position pos_b(101);
    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(id(pos, "furude")))
                                                   ->add(pipeSep(pos, "|:"))
                                                   ->deliver());
    builder.addArith(1, pos_a, (new grammar::TokenSequence(id(pos_a, "houjou")))
                                                     ->add(colon(pos_a))
                                                     ->add(pipeElement(pos_a))
                                                     ->add(op(pos_a, "*"))
                                                     ->add(pipeElement(pos_a))
                                                     ->deliver());
    builder.addReturn(1, pos_b, (new grammar::TokenSequence(id(pos_b, "houjou")))
                                                     ->deliver());

    builder.buildAndClear().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "[ pipeline ]")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "furude")
                (pos, OPERAND)
                    (BLOCK_BEGIN)
                        (pos_a, NAME_DEF, "houjou")
                            (pos_a, BINARY_OP, "*")
                                (pos_a, OPERAND)
                                    (pos_a, PIPE_ELEMENT)
                                (pos_a, OPERAND)
                                    (pos_a, PIPE_ELEMENT)
                        (pos_b, RETURN)
                            (pos_b, IDENTIFIER, "houjou")
                    (BLOCK_END)
        (BLOCK_END)
    ;
}

TEST_F(PipelineTest, MultiLinesPipelineSeparatedByFilterBreak)
{
    misc::position pos(2);
    misc::position pos_a(200);
    misc::position pos_b(201);
    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(id(pos, "furude")))
                                                   ->add(pipeSep(pos, "|?"))
                                                   ->deliver());
    builder.addArith(1, pos_a, (new grammar::TokenSequence(pipeElement(pos_a)))
                                                     ->add(op(pos_a, "<"))
                                                     ->add(pipeElement(pos_a))
                                                     ->deliver());

    builder.buildAndClear().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "[ |? ]")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "furude")
                (pos, OPERAND)
                    (pos_a, BINARY_OP, "<")
                        (pos_a, OPERAND)
                            (pos_a, PIPE_ELEMENT)
                        (pos_a, OPERAND)
                            (pos_a, PIPE_ELEMENT)
        (BLOCK_END)
    ;
}

TEST_F(PipelineTest, MultiLinesPipelineSeparatedByFilterBlock)
{
    misc::position pos(3);
    misc::position pos_a(300);
    misc::position pos_b(301);
    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(id(pos, "furude")))
                                                   ->add(pipeSep(pos, "|?"))
                                                   ->deliver());
    builder.addArith(1, pos_a, (new grammar::TokenSequence(pipeElement(pos_a)))
                                                     ->add(op(pos_a, "*"))
                                                     ->add(pipeElement(pos_a))
                                                     ->deliver());
    builder.addReturn(1, pos_b, (new grammar::TokenSequence(id(pos_b, "houjou")))
                                                     ->deliver());

    builder.buildAndClear();
    ASSERT_TRUE(error::hasError());

    std::vector<InvalidIndentRec> recs(getInvalidIndentRecs());
    ASSERT_EQ(1, recs.size());
    EXPECT_EQ(pos_b, recs[0].pos);
}
