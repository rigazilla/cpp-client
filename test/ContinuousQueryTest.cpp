/*
 * QueryTest.cpp
 *
 *  Created on: Apr 6, 2016
 *      Author: rigazilla
 */
#include <infinispan/hotrod/BasicTypesProtoStreamMarshaller.h>
#include "addressbook.pb.h"
#include "bank.pb.h"
#include <infinispan/hotrod/ProtoStreamMarshaller.h>
#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "infinispan/hotrod/RemoteCache.h"
#include "infinispan/hotrod/Version.h"
#include "infinispan/hotrod/Query.h"
#include "infinispan/hotrod/QueryUtils.h"
#include "infinispan/hotrod/CacheClientListener.h"
#include "infinispan/hotrod/ContinuousQueryListener.h"

#include "infinispan/hotrod/JBasicMarshaller.h"
#include <vector>
#include <tuple>

#define PROTOBUF_METADATA_CACHE_NAME "___protobuf_metadata"
#define ERRORS_KEY_SUFFIX  ".errors"

using namespace infinispan::hotrod;
using namespace org::infinispan::query::remote::client;

std::string read(std::string file)
{
    std::ifstream t(file);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}


int main(int argc, char** argv) {

    int result=0;
    std::cout << "Tests for Query" << std::endl;
    ConfigurationBuilder builder;
    builder.addServer().host(argc > 1 ? argv[1] : "127.0.0.1").port(11222);
    builder.protocolVersion(Configuration::PROTOCOL_VERSION_26);
    builder.balancingStrategyProducer(nullptr);
    RemoteCacheManager cacheManager(builder.build(), false);
    cacheManager.start();
    //initialize server-side serialization
    auto *km = new BasicTypesProtoStreamMarshaller<std::string>();
    auto *vm = new BasicTypesProtoStreamMarshaller<std::string>();


    RemoteCache<std::string, std::string> metadataCache = cacheManager.getCache<std::string, std::string>(
            km, &Marshaller<std::string>::destroy, vm, &Marshaller<std::string>::destroy,PROTOBUF_METADATA_CACHE_NAME, false);


    metadataCache.put("sample_bank_account/bank.proto", read("query_proto/bank.proto"));
    if (metadataCache.containsKey(ERRORS_KEY_SUFFIX))
    {
      std::cerr << "fail: error in registering .proto model" << std::endl;
      result=-1;
      return result;
    }

    auto *testkm = new BasicTypesProtoStreamMarshaller<int>();
    auto *testvm = new ProtoStreamMarshaller<sample_bank_account::User>();
    RemoteCache<int, sample_bank_account::User> testCache = cacheManager.getCache<int, sample_bank_account::User>(
            testkm, &Marshaller<int>::destroy, testvm, &Marshaller<sample_bank_account::User>::destroy, false);
    testCache.clear();

    ContinuousQueryListener<int, sample_bank_account::User> cql(testCache, "from sample_bank_account.User");
    std::promise<void> promise;
    int createdCount=0, changedCount=0, removedCount=0;

    std::function<void(int, sample_bank_account::User)> join =[&promise, &createdCount](int k, sample_bank_account::User u)
		{
    	  std::cout << "JOINING: key="<< u.id() << " value="<< u.name() << std::endl;
    	  ++createdCount;
		};

    std::function<void(int, sample_bank_account::User)> leave =[&promise, &removedCount](int k, sample_bank_account::User u)
		{
    	  std::cout << "LEAVING: key="<< u.id() << " value="<< u.name() << std::endl;
    	  ++removedCount;
          promise.set_value();
		};

    std::function<void(int, sample_bank_account::User)> change =[&promise, &changedCount](int k, sample_bank_account::User u)
		{
    	  std::cout << "CHANGING: key="<< u.id() << " value="<< u.name() << std::endl;
    	  ++changedCount;
		};

    cql.setJoiningListener(join);
    cql.setLeavingListener(leave);
    cql.setUpdatedListener(change);
    std::vector<char> param;

    std::string qString("from sample_bank_account.User");

    BasicTypesProtoStreamMarshaller<std::string> paramMarshaller;

    std::vector<std::vector<char>> filterFactoryParams;
    paramMarshaller.marshall(qString, param);
    filterFactoryParams.push_back(param);
    std::vector<std::vector<char> > converterFactoryParams;
    char CONTINUOUS_QUERY_FILTER_FACTORY_NAME[]="continuous-query-filter-converter-factory";
    CacheClientListener<int, sample_bank_account::User> cl(testCache);
    cl.filterFactoryName=std::vector<char>(CONTINUOUS_QUERY_FILTER_FACTORY_NAME, CONTINUOUS_QUERY_FILTER_FACTORY_NAME+strlen(CONTINUOUS_QUERY_FILTER_FACTORY_NAME));
    cl.converterFactoryName=std::vector<char>(CONTINUOUS_QUERY_FILTER_FACTORY_NAME, CONTINUOUS_QUERY_FILTER_FACTORY_NAME+strlen(CONTINUOUS_QUERY_FILTER_FACTORY_NAME));

    testCache.addContinuousQueryListener(cql);

    sample_bank_account::User_Address a;
    sample_bank_account::User user1;
    user1.set_id(3);
    user1.set_name("Tom");
    user1.set_surname("Cat");
    user1.set_gender(sample_bank_account::User_Gender_MALE);
    sample_bank_account::User_Address * addr= user1.add_addresses();
    addr->set_street("Via Roma");
    addr->set_number(3);
    addr->set_postcode("202020");
    testCache.put(3, user1);

    user1.set_id(4);
    user1.set_name("Jerry");
    user1.set_surname("Mouse");
    user1.set_gender(sample_bank_account::User_Gender_MALE);
    testCache.put(4, user1);

    user1.set_id(4);
    user1.set_name("Mickey");
    testCache.put(4, user1);

    testCache.remove(3);

    if (std::future_status::timeout==promise.get_future().wait_for(std::chrono::seconds(30)))
    {
      std::cout << "FAIL: Events and Listeners on Create (Timeout)" << std::endl;
      return -1;
    }

    if (createdCount!=2 || changedCount!=1 || removedCount!=1)
    {
    	std::cout << "FAIL: (createdCount, changedCount, removedCount) is  ("
    			<< createdCount << ", " << changedCount << ", " << removedCount << ")"
				<< "  should be (2,1,1)" << std::endl;
    	return -1;
    }

    std::cout << "PASS: continuous query" << std::endl;
    cacheManager.stop();
    return result;

}

