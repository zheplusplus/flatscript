#ifndef __STEKIN_TEST_DATA_TREES_H__
#define __STEKIN_TEST_DATA_TREES_H__

#include <list>
#include <string>
#include <ostream>
#include <algorithm>
#include <gtest/gtest.h>

#include <util/pointer.h>

#include "data-node.h"

namespace test {

    template <typename _NodeData, typename _FinalType>
    struct DataTreeTempl {
        DataTreeTempl& operator()(NodeType const& type
                                , _NodeData const& sub
                                , std::string const& str)
        {
            _nodes.push_back(util::mkptr(new StringNodeTempl<_NodeData>(type, sub, str)));
            return *this;
        }

        DataTreeTempl& operator()(NodeType const& type, _NodeData const& sub)
        {
            _nodes.push_back(util::mkptr(new NothingNodeTempl<_NodeData>(type, sub)));
            return *this;
        }
    protected:
        DataTreeTempl() {}
    public:
        static _FinalType& expectOne()
        {
            static _FinalType expect;
            return expect;
        }

        static _FinalType& actualOne()
        {
            static _FinalType actual;
            return actual;
        }

        static void verify()
        {
            _verify();
            expectOne()._nodes.clear();
            actualOne()._nodes.clear();
        }
    private:
        static void _verify()
        {
            ASSERT_EQ(expectOne()._nodes.size(), actualOne()._nodes.size());
            auto expect_iter = expectOne()._nodes.begin();
            auto actual_iter = actualOne()._nodes.begin();
            for (; expectOne()._nodes.end() != expect_iter; ++expect_iter, ++actual_iter) {
                EXPECT_EQ(**expect_iter, **actual_iter)
                    << "------------- [" << std::distance(expectOne()._nodes.begin(), expect_iter)
                    << "]" << std::endl;
            }
        }

        std::list<util::sptr<DataNodeTempl<_NodeData> const>> _nodes;
    };

}

#endif /* __STEKIN_TEST_DATA_TREES_H__ */
