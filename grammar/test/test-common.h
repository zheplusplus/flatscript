#ifndef __STEKIN_GRAMMAR_TEST_TEST_COMMON_H__
#define __STEKIN_GRAMMAR_TEST_TEST_COMMON_H__

#include <semantic/function.h>
#include <semantic/filter.h>
#include <semantic/compiling-space.h>
#include <output/node-base.h>
#include <output/function.h>
#include <misc/pos-type.h>
#include <test/data-node.h>
#include <test/data-trees.h>

#include "../node-base.h"
#include "../function.h"
#include "../clauses.h"

namespace test {

    extern std::map<std::string, grammar::TokenType> const IMAGE_TYPE_MAP;

    util::sptr<semantic::Filter> mkfilter();
    util::sref<semantic::SymbolTable> nulSymbols();
    semantic::CompilingSpace& nulSpace();

    struct TestClause
        : grammar::ClauseBase
    {
        TestClause()
            : grammar::ClauseBase(-1)
            , filter(nullptr)
        {}

        void compile()
        {
            filter = std::move(_block.compile());
        }

        util::sptr<semantic::Filter> filter;

        void deliver() {}
    };

    struct GrammarData {
        misc::position const pos;
        int const int_val;

        GrammarData(misc::position const& ps, int iv)
            : pos(ps)
            , int_val(iv)
        {}

        explicit GrammarData(misc::position const ps)
            : pos(ps)
            , int_val(-1)
        {}

        GrammarData()
            : pos(-1)
            , int_val(-1)
        {}

        bool operator==(GrammarData const& rhs) const
        {
            return pos == rhs.pos && int_val == rhs.int_val;
        }

        std::string str() const;
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

    extern NodeType const BOOLEAN;
    extern NodeType const INTEGER;
    extern NodeType const FLOATING;
    extern NodeType const STRING;
    extern NodeType const IDENTIFIER;
    extern NodeType const THIS;
    extern NodeType const LIST_BEGIN;
    extern NodeType const LIST_END;

    extern NodeType const UNDEFINED;
    extern NodeType const PRE_UNARY_OP;
    extern NodeType const BINARY_OP;
    extern NodeType const CONDITIONAL;
    extern NodeType const OPERAND;
    extern NodeType const PIPE_ELEMENT;
    extern NodeType const PIPE_INDEX;
    extern NodeType const PIPE_KEY;
    extern NodeType const PIPE_RESULT;

    extern NodeType const CALL_BEGIN;
    extern NodeType const CALL_END;
    extern NodeType const ARGUMENTS;
    extern NodeType const ASYNC_PLACEHOLDER_BEGIN;
    extern NodeType const ASYNC_PLACEHOLDER_END;
    extern NodeType const ASYNC_CALL;

    extern NodeType const LIST_SLICE_BEGIN;
    extern NodeType const LIST_SLICE_END;

    extern NodeType const DICT_BEGIN;
    extern NodeType const DICT_END;
    extern NodeType const DICT_ITEM;

    extern NodeType const NAME_DEF;
    extern NodeType const ARITHMETICS;
    extern NodeType const RETURN;
    extern NodeType const RETURN_NOTHING;
    extern NodeType const IMPORT;
    extern NodeType const EXPORT;
    extern NodeType const EXPORT_VALUE;
    extern NodeType const ATTR_SET;

    extern NodeType const REGULAR_ASYNC_PARAM_INDEX;
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
        : testing::Test
    {
        void SetUp();
        void TearDown();
    };

}

#endif /* __STEKIN_GRAMMAR_TEST_TEST_COMMON_H__ */
