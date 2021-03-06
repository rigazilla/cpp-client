#include <iostream>

#include "hotrod/impl/transport/tcp/RoundRobinBalancingStrategy.h"

namespace infinispan {
namespace hotrod {
namespace transport {

FailOverRequestBalancingStrategy* RoundRobinBalancingStrategy::newInstance() {
    return new RoundRobinBalancingStrategy();
}

RoundRobinBalancingStrategy::RoundRobinBalancingStrategy() : index(0) {}

void RoundRobinBalancingStrategy::setServers(const std::vector<transport::InetSocketAddress>& s) {
    servers = s;
    // keep the old index if possible so that we don't produce more requests for the first server
    if (index >= servers.size()) {
       index = 0;
    }
}

const transport::InetSocketAddress& RoundRobinBalancingStrategy::nextServer() {
    const transport::InetSocketAddress& server = getServerByIndex(index++);
    if (index >= servers.size()) {
       index = 0;
    }
    return server;
}

const transport::InetSocketAddress& RoundRobinBalancingStrategy::getServerByIndex(size_t pos) {
    const transport::InetSocketAddress& server = servers[pos];
    return server;
}

}}} /* namespace */
