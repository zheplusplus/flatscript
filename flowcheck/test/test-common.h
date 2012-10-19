#ifndef __STEKIN_FLOWCHECK_TEST_TEST_COMMON_H__
#define __STEKIN_FLOWCHECK_TEST_TEST_COMMON_H__

#include <proto/node-base.h>
#include <proto/block.h>
#include <misc/pos-type.h>
#include <test/data-node.h>
#include <test/data-trees.h>

namespace test {

    struct FlowcheckData {
        misc::position const pos;
        int const size;

        FlowcheckData(misc::position const& ps, int s)
            : pos(ps)
            , size(s)
        {}

        explicit FlowcheckData(misc::position const ps)
            : pos(ps)
            , size(-1)
        {}

        FlowcheckData()
            : size(-1)
        {}

        bool operator==(FlowcheckData const& rhs) const
        {
            return pos == rhs.pos && size == rhs.size;
        }
    };

    struct DataTree
        : public DataTreeTempl<FlowcheckData, DataTree>
    {
        typedef DataTreeTempl<FlowcheckData, DataTree> BaseType;

        DataTree& operator()(misc::position const& pos
                           , NodeType const& type
                           , std::string const& str);
        DataTree& operator()(misc::position const& pos
                           , NodeType const& type
                           , std::string const& str
                           , int size);
        DataTree& operator()(NodeType const& type);
        DataTree& operator()(NodeType const& type, std::string const& str);
        DataTree& operator()(misc::position const& pos, NodeType const& type);
        DataTree& operator()(misc::position const& pos, NodeType const& type, int size);
    };

    extern NodeType const BOOLEAN;
    extern NodeType const INTEGER;
    extern NodeType const FLOATING;
    extern NodeType const STRING;
    extern NodeType const LIST;
    extern NodeType const BINARY_OP;
    extern NodeType const PRE_UNARY_OP;
    extern NodeType const CALL;
    extern NodeType const REFERENCE;
    extern NodeType const LIST_ELEMENT;
    extern NodeType const LIST_INDEX;

    extern NodeType const LIST_PIPELINE;
    extern NodeType const PIPE_MAP;
    extern NodeType const PIPE_FILTER;

    extern NodeType const LIST_SLICE;
    extern NodeType const LIST_SLICE_DEFAULT;

    extern NodeType const DICT_BEGIN;
    extern NodeType const DICT_END;
    extern NodeType const DICT_ITEM;

    extern NodeType const NAME_DEF;

    extern NodeType const STATEMENT;
    extern NodeType const ARITHMETICS;
    extern NodeType const RETURN;
    extern NodeType const RETURN_NOTHING;
    extern NodeType const IMPORT;
    extern NodeType const ATTR_SET;

    extern NodeType const FUNC_DECL;
    extern NodeType const PARAMETER;

    extern NodeType const SCOPE_BEGIN;
    extern NodeType const SCOPE_END;

    extern NodeType const BRANCH;

    struct FlowcheckTest
        : public testing::Test
    {
        void SetUp();
        void TearDown();
    };

}

std::ostream& operator<<(std::ostream& os, test::FlowcheckData const& data);

#endif /* __STEKIN_FLOWCHECK_TEST_TEST_COMMON_H__ */
