	#pragma once

#include <functional>
#include "system_tick_hal.h"

#include "system_error.h"

typedef uint16_t product_id_t;
typedef uint16_t product_firmware_version_t;

namespace particle { namespace protocol {

#ifndef PRODUCT_ID
#define PRODUCT_ID (0xffff)
#endif

#ifndef PRODUCT_FIRMWARE_VERSION
#define PRODUCT_FIRMWARE_VERSION (0xffff)
#endif

#define MAX_SUBSCRIPTIONS (6)       // 2 system and 4 application

enum ProtocolError
{
    /* 00 */ NO_ERROR,
    /* 01 */ PING_TIMEOUT,
    /* 02 */ IO_ERROR,  // too generic, discontinue using this.  Perfer/add a specific one below
    /* 03 */ INVALID_STATE,
    /* 04 */ INSUFFICIENT_STORAGE,
    /* 05 */ MALFORMED_MESSAGE,
    /* 06 */ DECRYPTION_ERROR,
    /* 07 */ ENCRYPTION_ERROR,
    /* 08 */ AUTHENTICATION_ERROR,
    /* 09 */ BANDWIDTH_EXCEEDED,
    /* 10 */ MESSAGE_TIMEOUT,
    /* 11 */ MISSING_MESSAGE_ID,
    /* 12 */ MESSAGE_RESET,
    /* 13 */ SESSION_RESUMED,
    /* 14 */ IO_ERROR_FORWARD_MESSAGE_CHANNEL,
    /* 15 */ IO_ERROR_SET_DATA_MAX_EXCEEDED,
    /* 16 */ IO_ERROR_PARSING_SERVER_PUBLIC_KEY,
    /* 17 */ IO_ERROR_GENERIC_ESTABLISH,
    /* 18 */ IO_ERROR_GENERIC_RECEIVE,
    /* 19 */ IO_ERROR_GENERIC_SEND,
    /* 20 */ IO_ERROR_GENERIC_MBEDTLS_SSL_WRITE,
    /* 21 */ IO_ERROR_DISCARD_SESSION,
    /* 21 */ IO_ERROR_LIGHTSSL_BLOCKING_SEND,
    /* 22 */ IO_ERROR_LIGHTSSL_BLOCKING_RECEIVE,
    /* 23 */ IO_ERROR_LIGHTSSL_RECEIVE,
    /* 24 */ IO_ERROR_LIGHTSSL_HANDSHAKE_NONCE,
    /* 25 */ IO_ERROR_LIGHTSSL_HANDSHAKE_RECV_KEY,
    /* 26 */ NOT_IMPLEMENTED,

    /*
     * NOTE: when adding more ProtocolError codes, be sure to update toSystemError() in protocol_defs.cpp
     */
    UNKNOWN = 0x7FFFF
};

// Converts protocol error to system error code
system_error_t toSystemError(ProtocolError error);

typedef uint16_t chunk_index_t;

const chunk_index_t NO_CHUNKS_MISSING = 65535;
const chunk_index_t MAX_CHUNKS        = 65535;
const size_t MISSED_CHUNKS_TO_SEND    = 40u;
const size_t MINIMUM_CHUNK_INCREASE   = 2u;
const size_t MAX_EVENT_TTL_SECONDS    = 16777215;
const size_t MAX_OPTION_DELTA_LENGTH  = 12;
#if PLATFORM_ID<2
    const size_t MAX_FUNCTION_ARG_LENGTH = 64;
    const size_t MAX_FUNCTION_KEY_LENGTH = 12;
    const size_t MAX_VARIABLE_KEY_LENGTH = 12;
    const size_t MAX_EVENT_NAME_LENGTH   = 64;
    const size_t MAX_EVENT_DATA_LENGTH   = 255;
#else
    const size_t MAX_FUNCTION_ARG_LENGTH = 622;
    const size_t MAX_FUNCTION_KEY_LENGTH = 64;
    const size_t MAX_VARIABLE_KEY_LENGTH = 64;
    const size_t MAX_EVENT_NAME_LENGTH   = 64;
    const size_t MAX_EVENT_DATA_LENGTH   = 622;
#endif

// Timeout in milliseconds given to receive an acknowledgement for a published event
const unsigned SEND_EVENT_ACK_TIMEOUT = 20000;

#ifndef PROTOCOL_BUFFER_SIZE
    #if PLATFORM_ID<2
        #define PROTOCOL_BUFFER_SIZE 640
    #else
        #define PROTOCOL_BUFFER_SIZE 800
    #endif
#endif


namespace ChunkReceivedCode {
  enum Enum {
    OK = 0x44,
    BAD = 0x80
  };
}

enum DescriptionType {
    DESCRIBE_SYSTEM = 1<<0,            	// modules
    DESCRIBE_APPLICATION = 1<<1,       	// functions and variables
	DESCRIBE_METRICS = 1<<2,				// metrics/diagnostics
    DESCRIBE_DEFAULT = DESCRIBE_SYSTEM | DESCRIBE_APPLICATION,
	DESCRIBE_MAX = (1<<3)-1
};

namespace Connection
{
/**
 * Connection properties.
 */
enum Enum
{
    PING = 0, ///< Set keepalive interval.
    FAST_OTA = 1, ///< Enable/disable fast OTA.
    GOODBYE_ENABLED = 2 ///< Enable/disable Goodbye message.
};
}

typedef std::function<system_tick_t()> millis_callback;
typedef std::function<int()> callback;

const product_id_t UNDEFINED_PRODUCT_ID = product_id_t(-1);
const product_firmware_version_t UNDEFINED_PRODUCT_VERSION = product_firmware_version_t(-1);

namespace UpdateFlag {
enum Enum {
    ERROR         = 0x00,
    SUCCESS       = 0x01,
    VALIDATE_ONLY = 0x02,
    DONT_RESET    = 0x04
};
}

typedef uint32_t keepalive_source_t;

typedef struct
{
    uint16_t size;
    keepalive_source_t keepalive_source;
} connection_properties_t;

namespace KeepAliveSource {
enum Enum {
    USER   = 1<<0,   // set by user in wiring
    SYSTEM = 1<<1    // set by system
};
}

/**
 * Application state descriptor.
 */
class AppStateDescriptor {
public:
    /**
     * Construct an invalid descriptor.
     */
    AppStateDescriptor() :
            subscrCrc_(0),
            appDescCrc_(0),
            systemDescCrc_(0),
            protocolFlags_(0),
            valid_(false) {
    }

    /**
     * Construct a descriptor.
     *
     * @param subscriptionsCrc Checksum of the application subscriptions.
     * @param appDescribeCrc Checksum of the application functions and variables.
     * @param systemDescribeCrc Checksum of the system info.
     * @param protocolFlags Protocol flags (see `Protocol::Flags`).
     */
    AppStateDescriptor(uint32_t subscriptionsCrc, uint32_t appDescribeCrc, uint32_t systemDescribeCrc, uint32_t protocolFlags) :
            subscrCrc_(subscriptionsCrc),
            appDescCrc_(appDescribeCrc),
            systemDescCrc_(systemDescribeCrc),
            protocolFlags_(protocolFlags),
            valid_(true) {
    }

    /**
     * Returns `true` if `other` is equal to this descriptor, otherwise returns `false`.
     *
     * An invalid descriptor is never equal to any other descriptor.
     */
    bool operator==(const AppStateDescriptor& other) const {
        return (valid_ && other.valid_ && subscrCrc_ == other.subscrCrc_ && appDescCrc_ == other.appDescCrc_ &&
                systemDescCrc_ == other.systemDescCrc_ && protocolFlags_ == other.protocolFlags_);
    }

    /**
     * Returns `true` if `other` is not equal to this descriptor, otherwise returns `false`.
     *
     * An invalid descriptor is never equal to any other descriptor.
     */
    bool operator!=(const AppStateDescriptor& other) const {
        return !operator==(other);
    }

    /**
     * Returns `true` if this descriptor is valid.
     */
    bool isValid() const {
        return valid_;
    }

private:
    uint32_t subscrCrc_;
    uint32_t appDescCrc_;
    uint32_t systemDescCrc_;
    uint32_t protocolFlags_;
    bool valid_;
};

}} // namespace particle::protocol
