/*
 * AuthenticationConfiguration.h
 *
 *  Created on: Jan 27, 2017
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_AUTHENTICATIONCONFIGURATION_H_
#define INCLUDE_INFINISPAN_HOTROD_AUTHENTICATIONCONFIGURATION_H_
#if !defined _WIN32 && !defined _WIN64
#include <sasl/sasl.h>
#define HOTROD_SASL_CB_USER SASL_CB_USER
#define HOTROD_SASL_CB_AUTHNAME SASL_CB_AUTHNAME
#define HOTROD_SASL_CB_PASS SASL_CB_PASS
#define HOTROD_SASL_CB_GETREALM SASL_CB_GETREALM 
#define HOTROD_SASL_CB_GETPATH SASL_CB_GETPATH
#define HOTROD_SASL_CB_LIST_END SASL_CB_LIST_END
#else
#define HOTROD_SASL_CB_USER 0x4001
#define HOTROD_SASL_CB_AUTHNAME 0x4002
#define HOTROD_SASL_CB_PASS 0x4004
#define HOTROD_SASL_CB_GETREALM 0x4008 
#define HOTROD_SASL_CB_GETPATH 0x0003
#define HOTROD_SASL_CB_LIST_END 0x0000
typedef int(*sasl_callback_ft)(void *context, int id, const char **result, unsigned *len);
typedef struct sasl_callback {
    /* Identifies the type of the callback function.
    * Mechanisms must ignore callbacks with id's they don't recognize.
    */
    unsigned long id;
    sasl_callback_ft proc;   /* Callback function.  Types of arguments vary by 'id' */
    void *context;
} sasl_callback_t;
struct sasl_conn;
typedef struct sasl_conn sasl_conn_t;
typedef struct sasl_secret {
    unsigned long len;
    unsigned char data[1];		/* variable sized */
} sasl_secret_t;
#define SASL_OK          0   /* successful result */
#define SASL_BADPARAM   -7
#endif
namespace infinispan {
namespace hotrod {

/**
 * AuthenticationConfiguration object along with its factory AuthenticationConfigurationBuilder represent
 * used by ConfigurationBuilder for configuring RemoteCacheManager.
 *
 */

class AuthenticationConfiguration
{
public:
    AuthenticationConfiguration(std::vector<sasl_callback_t> callbackHandler, bool enabled, std::string saslMechanism, std::string serverFQDN)
                      : enabled(enabled), callbackHandler(callbackHandler), saslMechanism(saslMechanism), serverFQDN(serverFQDN) {}
    AuthenticationConfiguration() : enabled(false) {}
    bool isEnabled() const { return enabled; }

    const std::string& getSaslMechanism() const {
        return saslMechanism;
    }

    const std::vector<sasl_callback_t>& getCallbackHandler() const {
        return callbackHandler;
    }

    const std::string& getServerFqdn() const {
        return serverFQDN;
    }

private:
    bool enabled;
    std::vector<sasl_callback_t> callbackHandler;
    std::string saslMechanism;
    std::string serverFQDN;
};

}}

#endif /* INCLUDE_INFINISPAN_HOTROD_AUTHENTICATIONCONFIGURATION_H_ */
