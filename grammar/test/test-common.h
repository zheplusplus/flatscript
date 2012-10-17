#ifndef __STEKIN_GRAMMAR_TEST_TEST_COMMON_H__
#define __STEKIN_GRAMMAR_TEST_TEST_COMMON_H__

#include <flowcheck/node-base.h>
#include <misc/pos-type.h>
#include <test/data-node.h>
#include <test/data-trees.h>

namespace test {

    util::sptr<flchk::Filter> mkfilter();
    util::sref<flchk::SymbolTable> nulSymbols();

    struct GrammarData {
        misc::position const pos;
        int const func_arg_size;

        GrammarData(misc::position const& ps, int arg_size)
            : pos(ps)
            , func_arg_size(arg_size)
        {}

        explicit GrammarData(misc::position const ps)
            : pos(ps)
            , func_arg_size(-1)
        {}

        GrammarData()
            : pos(-1)
            , func_arg_size(-1)
        {}

        bool operator==(GrammarData const& rhs) const
        {
            return pos == rhs.pos && func_arg_size == rhs.func_arg_size;
        }
    };

    struct DataTree
        : public DataTreeTempl<GrammarData, DataTree>
    {
        typedef DataTreeTempl<GrammarData, DataTree> BaseType;

        DataTree& operator()(misc::position const& pos
                           , NodeType const& type
                           , std::string const& str);
        DataTree& operator()(misc::position const& pos
                           , NodeType const& type
                           , std::string const& str
                           , int func_arg_size);
        DataTree& operator()(NodeType const& type);
        DataTree& operator()(misc::position const& pos, NodeType const& type);
        DataTree& operator()(misc::position const& pos, NodeType const& type, int list_size);
    };

    extern std::string const VAR_DEF_FILTERED;
    extern std::string const FUNC_DEF_FILTERED;

    extern NodeType const BOOLEAN;
    extern NodeType const INTEGER;
    extern NodeType const FLOATING;
    extern NodeType const STRING;
    extern NodeType const REFERENCE;
    extern NodeType const LIST;
    extern NodeType const LIST_ELEMENT;
    extern NodeType const LIST_INDEX;

    extern NodeType const BINARY_OP;
    extern NodeType const PRE_UNARY_OP;
    extern NodeType const LIST_PIPELINE;
    extern NodeType const PIPE_MAP;
    extern NodeType const PIPE_FILTER;

    extern NodeType const CALL;

    extern NodeType const VAR_DEF;
    extern NodeType const ARITHMETICS;
    extern NodeType const RETURN;
    extern NodeType const RETURN_NOTHING;

    extern NodeType const FUNC_DEF;
    extern NodeType const PARAMETER;

    extern NodeType const BRANCH;
    extern NodeType const BRANCH_CONSQ_ONLY;
    extern NodeType const BRANCH_ALTER_ONLY;
    extern NodeType const CONSEQUENCE;
    extern NodeType const ALTERNATIVE;

    extern NodeType const BLOCK_BEGIN;
    extern NodeType const BLOCK_END;

    struct GrammarTest
        : public testing::Test
    {
        void SetUp();
        void TearDown();
    };

}

std::ostream& operator<<(std::ostream& os, test::GrammarData const& data);

#endif /* __STEKIN_GRAMMAR_TEST_TEST_COMMON_H__ */
