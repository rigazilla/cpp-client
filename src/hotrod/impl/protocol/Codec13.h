#ifndef ISPN_HOTROD_PROTOCOL_CODEC13_H
#define ISPN_HOTROD_PROTOCOL_CODEC13_H


#include <set>
#include <map>
#include <string>
#include "hotrod/impl/protocol/Codec12.h"

namespace infinispan {
namespace hotrod {
namespace protocol {

class Codec13: public Codec12 {

public:
    HeaderParams& writeHeader(
            infinispan::hotrod::transport::Transport& transport,
            HeaderParams& params) const;

    Codec13() {
    }

    friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC13_H
