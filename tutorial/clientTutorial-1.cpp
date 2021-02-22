#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include <infinispan/hotrod/RemoteCache.h>
#include <iostream>
int main () {
	ConfigurationBuilder builder;
	// Configure a cache manager to connect with hotrod ver 2.8 to
	// server 127.0.0.1:11222 without starting it
        builder.protocolVersion(Configuration::PROTOCOL_VERSION_28);
        builder.addServer().host("127.0.0.1").port(11222);
        RemoteCacheManager cacheManager(builder.build(), false);

	// Start the cacheManager and get the default cache and use it for entries
	// of type <string,string>
	cacheManager.start();
	RemoteCache<std::string, std::string> cache = cacheManager.getCache<std::string, std::string>();

	std::string key = std::string("clientTutorial-1");
        std::string value = std::string("value for clientTutorial-1");

	// Storing a key,value entry in the cache
	cache.put(key, value);

	// Getting the entry from the cache
	std::string *valueFromCache (cache.get(key));
        std::cout << "Value got from cache for entry: key=\"" << key << "\"value=" << *valueFromCache << std::endl;

	// Clean up
	delete valueFromCache;
	cacheManager.stop();
	return 1;
}
