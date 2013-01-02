#ifndef __STEKIN_TEST_DATA_NODE_H__
#define __STEKIN_TEST_DATA_NODE_H__

#include <string>
#include <ostream>

#include <misc/pos-type.h>
#include <util/pointer.h>

namespace test {

    template <typename NodeData> struct NothingNodeTempl;
    template <typename NodeData> struct StringNodeTempl;

    struct NodeType {
        std::string const type_img;

        explicit NodeType(std::string const& name)
            : type_img(name)
        {}

        bool operator==(NodeType const& rhs) const
        {
            return type_img == rhs.type_img;
        }
    };

    template <typename NodeData>
    struct DataNodeTempl {
        NodeType const type_img;
        NodeData const sub_data;
    public:
        virtual ~DataNodeTempl() {}

        bool operator==(DataNodeTempl const& rhs) const
        {
            return type_img == rhs.type_img && sub_data == rhs.sub_data && cmp(rhs);
        }

        virtual std::ostream& print(std::ostream& os) const = 0;

        virtual bool cmp(DataNodeTempl<NodeData> const& rhs) const = 0;

        virtual bool cmp_no_data(NothingNodeTempl<NodeData> const&) const
        {
            return false;
        }

        virtual bool cmp_str_data(StringNodeTempl<NodeData> const&) const
        {
            return false;
        }
    protected:
        DataNodeTempl(NodeType const& timg, NodeData const& sdata)
            : type_img(timg)
            , sub_data(sdata)
        {}
    };

    template <typename _NodeData>
    struct NothingNodeTempl
        : public DataNodeTempl<_NodeData>
    {
        NothingNodeTempl(NodeType const& type_img, _NodeData const& sub_data)
            : DataNodeTempl<_NodeData>(type_img, sub_data)
        {}

        std::ostream& print(std::ostream& os) const
        {
            return os;
        }

        bool cmp(DataNodeTempl<_NodeData> const& rhs) const
        {
            return rhs.cmp_no_data(*this);
        }

        bool cmp_no_data(NothingNodeTempl<_NodeData> const&) const
        {
            return true;
        }
    };

    template <typename _NodeData>
    struct StringNodeTempl
        : public DataNodeTempl<_NodeData>
    {
        std::string const data;

        StringNodeTempl(NodeType const& type_img, _NodeData const& sub_data, std::string const& d)
            : DataNodeTempl<_NodeData>(type_img, sub_data)
            , data(d)
        {}

        std::ostream& print(std::ostream& os) const
        {
            return os << " string data: " << data;
        }

        bool cmp(DataNodeTempl<_NodeData> const& rhs) const
        {
            return rhs.cmp_str_data(*this);
        }

        bool cmp_str_data(StringNodeTempl<_NodeData> const& lhs) const
        {
            return data == lhs.data;
        }
    };

}

template <typename NodeData>
std::ostream& operator<<(std::ostream& os, util::sref<test::DataNodeTempl<NodeData> const> node)
{
    return node->print(os <<  "node type: " << node->type_img.type_img) << std::endl
               << ":: sub data: " << node->sub_data.str();
}

template <typename NodeData>
std::ostream& operator<<(std::ostream& os, test::DataNodeTempl<NodeData> const& node)
{
    return operator<<(os, util::mkref(node));
}

#endif /* __STEKIN_TEST_DATA_NODE_H__ */
