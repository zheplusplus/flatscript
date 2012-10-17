#ifndef __STEKIN_PARSER_TEST_TEST_COMMON_H__
#define __STEKIN_PARSER_TEST_TEST_COMMON_H__

#include <misc/pos-type.h>
#include <test/data-node.h>
#include <test/data-trees.h>

namespace test {

    struct ParserData {
        int const indent_level;
        misc::position const pos;

        ParserData(int level, misc::position const& ps)
            : indent_level(level)
            , pos(ps)
        {}

        explicit ParserData(misc::position const ps)
            : indent_level(-1)
            , pos(ps)
        {}

        bool operator==(ParserData const& rhs) const
        {
            return indent_level == rhs.indent_level && pos == rhs.pos;
        }
    };

    struct DataTree
        : public DataTreeTempl<ParserData, DataTree>
    {
        typedef DataTreeTempl<ParserData, DataTree> BaseType;

        DataTree& operator()(misc::position const& pos
                           , NodeType const& type
                           , std::string const& str);
        DataTree& operator()(misc::position const& pos
                           , int indent
                           , NodeType const& type
                           , std::string const& str);
        DataTree& operator()(misc::position const& pos, NodeType const& type);
        DataTree& operator()(misc::position const& pos, int indent, NodeType const& type);
    };

    extern NodeType const BOOLEAN;
    extern NodeType const INTEGER;
    extern NodeType const FLOATING;
    extern NodeType const STRING;
    extern NodeType const IDENTIFIER;
    extern NodeType const LIST_BEGIN;
    extern NodeType const LIST_END;
    extern NodeType const LIST_ELEMENT;
    extern NodeType const LIST_INDEX;

    extern NodeType const LIST_PIPELINE;
    extern NodeType const PIPE_BEGIN;
    extern NodeType const PIPE_END;
    extern NodeType const PIPE_MAP;
    extern NodeType const PIPE_FILTER;

    extern NodeType const BINARY_OP_BEGIN;
    extern NodeType const BINARY_OP_END;
    extern NodeType const PRE_UNARY_OP_BEGIN;
    extern NodeType const PRE_UNARY_OP_END;
    extern NodeType const OPERAND;

    extern NodeType const FUNC_CALL_BEGIN;
    extern NodeType const FUNC_CALL_END;
    extern NodeType const ARGUMENT;

    extern NodeType const VAR_DEF;
    extern NodeType const ARITHMETICS;
    extern NodeType const RETURN;

    extern NodeType const FUNC_DEF_HEAD_BEGIN;
    extern NodeType const FUNC_DEF_HEAD_END;

    extern NodeType const CONDITION_BEGIN;
    extern NodeType const CONDITION_END;

    extern NodeType const BRANCH_IF;
    extern NodeType const BRANCH_IFNOT;
    extern NodeType const BRANCH_ELSE;

}

std::ostream& operator<<(std::ostream& os, test::ParserData const& data);

#endif /* __STEKIN_PARSER_TEST_TEST_COMMON_H__ */
