#ifndef __STEKIN_PROTO_OPERATION_H__
#define __STEKIN_PROTO_OPERATION_H__

#include <string>

namespace proto {

    std::string mapUnaryOp(std::string const& op);
    std::string mapBinaryOp(std::string const& op);

}

#endif /* __STEKIN_PROTO_OPERATION_H__ */
