#include <infinispan/hotrod/InetSocketAddress.h>
#include "hotrod/impl/protocol/Codec24.h"
#include "hotrod/impl/protocol/HotRodConstants.h"
#include "hotrod/impl/protocol/HeaderParams.h"
#include "hotrod/impl/transport/Transport.h"
#include "hotrod/impl/transport/TransportFactory.h"
#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <vector>

namespace infinispan {
namespace hotrod {

using transport::Transport;
using transport::InetSocketAddress;
using transport::TransportFactory;

namespace protocol {

HeaderParams& Codec24::writeHeader(Transport& transport, HeaderParams& params) const {
    return Codec20::writeHeader(transport, params, HotRodConstants::VERSION_24);
}

}}} // namespace infinispan::hotrod::protocol
