#ifndef __STEKIN_SEMANTIC_TEST_TEST_COMMON_H__
#define __STEKIN_SEMANTIC_TEST_TEST_COMMON_H__

#include <semantic/filter.h>
#include <semantic/function.h>
#include <output/node-base.h>
#include <output/block.h>
#include <output/function.h>
#include <misc/pos-type.h>
#include <test/data-node.h>
#include <test/data-trees.h>

namespace test {

    std::ostream& dummyos();
    util::sptr<output::Statement const> compile(
        semantic::Filter& f, util::sref<semantic::SymbolTable> sym);

    struct SemanticData {
        misc::position const pos;
        int const int_val;

        SemanticData(misc::position const& ps, int iv)
            : pos(ps)
            , int_val(iv)
        {}

        explicit SemanticData(misc::position const ps)
            : pos(ps)
            , int_val(-1)
        {}

        explicit SemanticData(int iv)
            : int_val(iv)
        {}

        SemanticData()
            : int_val(-1)
        {}

        bool operator==(SemanticData const& rhs) const
        {
            return pos == rhs.pos && int_val == rhs.int_val;
        }

        std::string str() const;
    };

    struct DataTree
        : DataTreeTempl<SemanticData, DataTree>
    {
        typedef DataTreeTempl<SemanticData, DataTree> BaseType;

        DataTree& operator()(misc::position const& pos
                           , NodeType const& type
                           , std::string const& str);
        DataTree& operator()(NodeType const& type
                           , std::string const& str
                           , int size);
        DataTree& operator()(NodeType const& type);
        DataTree& operator()(NodeType const& type, std::string const& str);
        DataTree& operator()(NodeType const& type, int value);
        DataTree& operator()(misc::position const& pos, NodeType const& type);
        DataTree& operator()(misc::position const& pos, NodeType const& type, int size);
    };

    extern NodeType const UNDEFINED;
    extern NodeType const BOOLEAN;
    extern NodeType const INTEGER;
    extern NodeType const FLOATING;
    extern NodeType const STRING;
    extern NodeType const LIST;
    extern NodeType const THIS;
    extern NodeType const BINARY_OP;
    extern NodeType const PRE_UNARY_OP;
    extern NodeType const CONDITIONAL;
    extern NodeType const REFERENCE;
    extern NodeType const IMPORTED_NAME;
    extern NodeType const PIPE_ELEMENT;
    extern NodeType const PIPE_INDEX;
    extern NodeType const PIPE_KEY;
    extern NodeType const PIPE_RESULT;

    extern NodeType const CALL;
    extern NodeType const FUNC_INVOKE;
    extern NodeType const ASYNC_REFERENCE;

    extern NodeType const ASYNC_PIPELINE;
    extern NodeType const SYNC_PIPELINE;
    extern NodeType const PIPELINE_CONTINUE;

    extern NodeType const LIST_SLICE;

    extern NodeType const DICT_BEGIN;
    extern NodeType const DICT_END;
    extern NodeType const DICT_ITEM;

    extern NodeType const ASYNC_RESULT_DEF;

    extern NodeType const STATEMENT;
    extern NodeType const ARITHMETICS;
    extern NodeType const RETURN;
    extern NodeType const EXPORT;
    extern NodeType const EXPORT_VALUE;
    extern NodeType const DEC_THIS;
    extern NodeType const BRANCH;

    extern NodeType const FUNCTION;
    extern NodeType const PARAMETER;
    extern NodeType const FWD_DECL;
    extern NodeType const COPY_PARAM_DECL;
    extern NodeType const REGULAR_ASYNC_RETURN;

    extern NodeType const EXC_THROW;
    extern NodeType const EXC_CALLBACK;

    extern NodeType const SCOPE_BEGIN;
    extern NodeType const SCOPE_END;

    struct SemanticTest
        : testing::Test
    {
        void SetUp();
        void TearDown();
    };

}

#endif /* __STEKIN_SEMANTIC_TEST_TEST_COMMON_H__ */
