#ifndef ISPN_HOTROD_PROTOCOL_CODEC21_H
#define ISPN_HOTROD_PROTOCOL_CODEC21_H


#include <set>
#include <map>
#include <string>
#include "hotrod/impl/protocol/Codec20.h"

namespace infinispan {
namespace hotrod {
namespace protocol {

class Codec21: public Codec20 {

public:
    HeaderParams& writeHeader(
            infinispan::hotrod::transport::Transport& transport,
            HeaderParams& params) const;

    Codec21() {
    }

    friend class CodecFactory;
};

}}} // namespace infinispan::hotrod::protocol

#endif // ISPN_HOTROD_PROTOCOL_CODEC21_H
