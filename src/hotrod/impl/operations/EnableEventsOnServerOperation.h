#pragma once
#include "hotrod/impl/operations/RetryOnFailureOperation.h"
#include "infinispan/hotrod/ClientEvent.h"
#include <random>
using namespace infinispan::hotrod::event;
namespace infinispan
{
    namespace hotrod
    {
        namespace operations {
            class EnableEventsOnServerOperation : public RetryOnFailureOperation<std::vector<ClientCacheEventData> > 
            {
            public:
                EnableEventsOnServerOperation(const Codec &codec, std::shared_ptr<TransportFactory> transportFactory
                    , const std::vector<char> cacheName, Topology& topologyId, int flags, std::vector<char> filterName
                    , std::vector<char> converterName, bool includeCurrentState
                    , std::vector<std::vector<char> > filterFactoryParams, std::vector<std::vector<char> > converterFactoryParams)
                    : RetryOnFailureOperation(codec, transportFactory, cacheName, topologyId, flags)
                    , listenerId(generateV4UUID()), filterName(filterName), converterName(converterName), includeCurrentState(includeCurrentState)
                    , filterFactoryParams(filterFactoryParams), converterFactoryParams(converterFactoryParams) {}
                void releaseTransport(transport::Transport*)
                {
                }

                transport::Transport& getTransport(int, const std::set<transport::InetSocketAddress>& failedServers)
                {
                    dedicatedTransport = transportFactory->getTransport(this->cacheName, failedServers).clone();
                    return *dedicatedTransport;
                }

                std::vector<ClientCacheEventData> executeOperation(transport::Transport& transport)
                {
                    std::vector<ClientCacheEventData> result;
                    ClientCacheEventData cced;
                    cced.isCustom = true;
                    cced.version = 42;
                    result.push_back(cced);
                    protocol::HeaderParams params = this->writeHeader(transport, ADD_CLIENT_LISTENER_REQUEST);
                    transport.writeArray(listenerId);
                    const Codec20& codec20 = dynamic_cast<const Codec20&>(codec);
                    codec20.writeClientListenerParams(transport, includeCurrentState, useRawData, filterName, converterName, filterFactoryParams, converterFactoryParams);
                    transport.flush();
                    bool readMore = true;
                    uint64_t respMessageId = 0;
                    try
                    {
                        do
                        {
                            uint8_t respOpCode = codec20.readAddEventListenerResponseType(transport, respMessageId);
                            // The response contains immediate event to process
                            if (isEvent(respOpCode))
                            {
                                EventHeaderParams params;
                                params.messageId = respMessageId;
                                params.opCode = respOpCode;
                                try
                                {
                                    uint8_t status = codec20.readPartialEventHeader(transport, params);
                                    if (!HotRodConstants::isSuccess(status))
                                        break;
                                    result.push_back(codec20.readEventAsData(transport, respOpCode));
                                }
                                catch (HotRodClientException e) {
                                    continue;
                                }
                            }
                            else
                            {
                                if (respMessageId != params.getMessageId() && respMessageId != 0) {
                                    std::ostringstream message;
                                    message << "Invalid message id. Expected " <<
                                        params.getMessageId() << " and received " << respMessageId;
                                    throw InvalidResponseException(message.str());
                                }
                                uint8_t status = codec20.readPartialHeader(transport, params, respOpCode);
                                isSucceeded = HotRodConstants::isSuccess(status);
                                readMore = false;
                            }
                        } while (readMore);
                    }
                    catch (const TransportException& ex)
                    {
                        isSucceeded = false;
                        transport::TcpTransport& tcpT = dynamic_cast<transport::TcpTransport&>(transport);
                        throw TransportException(tcpT.getServerAddress().getHostname(), tcpT.getServerAddress().getPort(), ex.what(), ex.getErrnum());
                    }
                    return result;
                }
    
                /**
                * Dedicated transport instance for adding client listener. This transport
                * is used to send events back to client and it's only released when the
                * client listener is removed.
                */
                Transport *dedicatedTransport;
                std::vector<char> listenerId;
                std::vector<char> filterName, converterName;
                bool includeCurrentState;
                bool useRawData;
                std::vector<std::vector<char> > filterFactoryParams, converterFactoryParams;
                bool isSucceeded;
            private:
                /* This function generate an random ID that seems a V4 UUID
                * it's not an implementation of UUID v4
                */
                std::vector<char> generateV4UUID()
                {
                    std::vector<char> tmp(16);
                    static std::default_random_engine e{};
                    static std::uniform_int_distribution<int> d{ 0,255 };
                    auto i = 0;
                    for (; i<16; i++)
                    {
                        tmp[i] = (unsigned char)d(e);
                    }
                    tmp[6] = (tmp[6] & 0x0F) | 0x40;
                    tmp[8] = (tmp[8] & 0x3F) | 0x80;
                    return tmp;
                }


            };
        }
    }
}

