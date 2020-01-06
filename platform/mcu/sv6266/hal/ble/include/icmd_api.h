/** 
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef _ICMD_API_H_
#define _ICMD_API_H_

/**
 * iCMD API: interface with programmer
 *
 */
// ICMD_GAP_DEVICE_CONFIG
#define GAP_DEVICE_CONFIG(\
    p_public_addr, IO_capability, Bond_flag, MITM, OOB_flag, key_distribution, reserved1, reserved2, \
        feature, develop_mode, low_power_level \
) \
icmd2status(ICMD_OP_GAP_DEVICE_CONFIG, \
    PTR(p_public_addr), U8(IO_capability), U8(Bond_flag), U8(MITM), U8(OOB_flag), U8(key_distribution), U8(reserved1), U8(reserved2), \
        U16(feature), U16(0), U8(develop_mode), U8(low_power_level))

// ICMD_GAP_SET_ADV_PARAM
#define GAP_SET_ADV_PARAM(\
    adv_interval, adv_type, own_addr_type, peer_addr_type, p_peer_addr, adv_channel_map, adv_filter, reserved \
) \
icmd2status(ICMD_OP_GAP_SET_ADV_PARAM,\
    U16(adv_interval), U8(adv_type), U8(own_addr_type), U8(peer_addr_type), PTR(p_peer_addr), U8(adv_channel_map), U8(adv_filter), U8(reserved))

// ICMD_GAP_SET_ADV_DATA
#define GAP_SET_ADV_DATA(\
    data_type, length, p_data\
)\
icmd2status(ICMD_OP_GAP_SET_ADV_DATA, \
    U8(data_type), U8(length), VPTR(p_data))

// ICMD_GAP_SET_ADV_ENABLE
#define GAP_SET_ADV_ENABLE(\
    enable, timeout_u16, reserved\
)\
icmd2status(ICMD_OP_GAP_SET_ADV_ENABLE, \
    U8(enable), U16(timeout_u16), U16(0), U8(reserved))

// ICMD_GAP_SET_SCAN_PARAM,
#define GAP_SET_SCAN_PARAM(\
    scan_type, scan_interval, scan_window, own_addr_type, filter_policy\
)\
icmd2status(ICMD_OP_GAP_SET_SCAN_PARAM, \
    U8(scan_type), U16(scan_interval), U16(scan_window), U8(own_addr_type), U8(filter_policy))

// ICMD_GAP_SET_SCAN_ENABLE,
#define GAP_SET_SCAN_ENABLE(\
    enable, filter_duplicate, timeout_u16\
)\
icmd2status(ICMD_OP_GAP_SET_SCAN_ENABLE, \
    U8(enable), U8(filter_duplicate), U16(timeout_u16), U16(0))

// ICMD_GAP_ADD_DEVICE_TO_WHITE_LIST,
#define GAP_ADD_DEVICE_TO_WHITE_LIST(\
    entry, addr_type, p_addr\
)\
icmd2status(ICMD_OP_GAP_ADD_DEVICE_TO_WHITE_LIST, \
    U8(entry), U8(addr_type), PTR(p_addr))

// ICMD_GAP_REMOVE_DEVICE_FROM_WHITE_LIST,
#define GAP_REMOVE_DEVICE_FROM_WHITE_LIST(\
    remove_flag\
)\
icmd2status(ICMD_OP_GAP_REMOVE_DEVICE_FROM_WHITE_LIST, \
    U8(remove_flag))

// ICMD_GAP_SET_TX_POWER,
#define GAP_SET_TX_POWER(\
    type, power\
)\
icmd2status(ICMD_OP_GAP_SET_TX_POWER, \
    U8(type), U8(power))

// ICMD_GAP_GEN_RANDOM_ADDR,
#define GAP_GEN_RANDOM_ADDR(\
    addr_type, \
    p_addr\
)\
icmd2status(ICMD_OP_GAP_GEN_RANDOM_ADDR, \
    U8(addr_type), \
    PTR(NULL), PTR(p_addr))

// ICMD_GAP_SET_RANDOM_ADDR,
#define GAP_SET_RANDOM_ADDR(\
    p_addr\
)\
icmd2status(ICMD_OP_GAP_SET_RANDOM_ADDR, \
    PTR(p_addr))

// ICMD_GAP_GET_PUBLIC_ADDR,
#define GAP_GET_PUBLIC_ADDR(\
    \
    p_return_addr\
)\
icmd2status(ICMD_OP_GAP_GET_PUBLIC_ADDR, \
    \
    PTR(p_return_addr))

// ICMD_GAP_GET_REMOTE_VERSION_INFO,
#define GAP_GET_REMOTE_VERSION_INFO(\
    \
    p_version, p_manufacturer_name, p_subversion\
)\
icmd2status(ICMD_OP_GAP_GET_REMOTE_VERSION_INFO, \
    \
    PTR(p_version), PTR(p_manufacturer_name), PTR(p_subversion))

// ICMD_GAP_CONNECTION_PARAM_UPD,
#define GAP_CONNECTION_PARAM_UPD(\
    conn_interval_min, conn_interval_max, slave_latency, supervision_timeout\
)\
icmd2status(ICMD_OP_GAP_CONNECTION_PARAM_UPD, \
    U16(conn_interval_min), U16(conn_interval_max), U16(slave_latency), U16(supervision_timeout))

// ICMD_GAP_DISCONNECT,
#define GAP_DISCONNECT(\
    reason\
)\
icmd2status(ICMD_OP_GAP_DISCONNECT, \
    U8(reason))

// ICMD_GAP_READ_RSSI,
#define GAP_READ_RSSI(\
    \
    p_rssi\
)\
icmd2status(ICMD_OP_GAP_READ_RSSI, \
    \
    PTR(p_rssi))

// ICMD_GAP_SECURITY_SET_KEY,
#define GAP_SECURITY_SET_KEY(\
    type, p_key\
)\
icmd2status(ICMD_OP_GAP_SECURITY_SET_KEY, \
    U8(type), PTR(p_key))

// ICMD_GAP_SECURITY,
#define GAP_SECURITY(\
    enable\
)\
icmd2status(ICMD_OP_GAP_SECURITY, \
    U8(enable))

// ICMD_GAP_SECURITY_PASSKEY_INPUT_CFM,
#define GAP_SECURITY_PASSKEY_INPUT_CFM(\
    type, tk_len, p_tk\
)\
icmd2status(ICMD_OP_GAP_SECURITY_PASSKEY_INPUT_CFM, \
    U8(type), U8(tk_len), VPTR(p_tk))

// ICMD_GATTS_DATABASE_RESET,
#define GATTS_DATABASE_RESET(\
)\
icmd2status(ICMD_OP_GATTS_DATABASE_RESET)

// ICMD_GATTS_ADD_SERVICE,
#define GATTS_ADD_SERVICE(\
    service_type, start_index, end_index, uuid_len, p_uuid, \
    p_reserved\
)\
icmd2status(ICMD_OP_GATTS_ADD_SERVICE, \
    U8(service_type), U8(start_index), U8(end_index), U8(uuid_len), VPTR(p_uuid), \
    PTR(p_reserved))

// ICMD_GATTS_ADD_INCLUDE_SERVICE,
#define GATTS_ADD_INCLUDE_SERVICE(\
    start_index, \
    p_reserved\
)\
icmd2status(ICMD_OP_GATTS_ADD_INCLUDE_SERVICE, \
    U8(start_index), \
    PTR(p_reserved))

// ICMD_GATTS_ADD_CHARACTERISTIC,
#define GATTS_ADD_CHARACTERISTIC(\
    characteristic_index, permit, property, is_variable_value_len, uuid_len, p_uuid, value_size, p_value, \
    p_reserved\
)\
icmd2status(ICMD_OP_GATTS_ADD_CHARACTERISTIC, \
    U8(characteristic_index), U16(permit), U8(property), U8(is_variable_value_len), U8(uuid_len), VPTR(p_uuid), U8(value_size), VPTR(p_value), \
    PTR(p_reserved))

// ICMD_GATTS_ADD_CHARACTERISTIC_DESCRIPTOR,
#define GATTS_ADD_CHARACTERISTIC_DESCRIPTOR(\
    characteristic_index, descriptior_index, permit, is_variable_value_len, uuid_len, p_uuid, value_size, p_value, \
    p_reserved\
)\
icmd2status(ICMD_OP_GATTS_ADD_CHARACTERISTIC_DESCRIPTOR, \
    U8(characteristic_index), U8(descriptior_index), U16(permit), U8(is_variable_value_len), U8(uuid_len), VPTR(p_uuid), U8(value_size), VPTR(p_value), \
    PTR(p_reserved))

// ICMD_GATTS_SET_VALUE,
#define GATTS_SET_VALUE(\
    index, value_size, p_value\
)\
icmd2status(ICMD_OP_GATTS_SET_VALUE, \
    U8(index), U8(value_size), VPTR(p_value))

// ICMD_GATTS_SERVER_INDICATE,
#define GATTS_SERVER_INDICATE(\
    indicate_type, index, value_size, p_value\
)\
icmd2status(ICMD_OP_GATTS_SERVER_INDICATE, \
    U8(indicate_type), U8(index), U8(value_size), VPTR(p_value))

// ICMD_GATTS_AUTHORIZE_CONFIRM,
#define GATTS_AUTHORIZE_CONFIRM(\
    index, authorize, error_code\
)\
icmd2status(ICMD_OP_GATTS_AUTHORIZE_CONFIRM, \
    U8(index), U8(authorize), U8(error_code))

// ICMD_GATTS_AUTHORIZE_CONFIRM_WITH_VALUE,
#define GATTS_AUTHORIZE_CONFIRM_WITH_VALUE(\
    index, value_size, p_value\
)\
icmd2status(ICMD_OP_GATTS_AUTHORIZE_CONFIRM_WITH_VALUE, \
    U8(index), U8(value_size), VPTR(p_value))

// ICMD_GATTS_PREPARE_WRITE_AUTHORIZE_CONFIRM,
#define GATTS_PREPARE_WRITE_AUTHORIZE_CONFIRM(\
    index, authorize, error_code\
)\
icmd2status(ICMD_OP_GATTS_PREPARE_WRITE_AUTHORIZE_CONFIRM, \
    U8(index), U8(authorize), U8(error_code))

// ICMD_GATTS_EXECUTE_WRITE_AUTHORIZE_CONFIRM,
#define GATTS_EXECUTE_WRITE_AUTHORIZE_CONFIRM(\
    authorize, error_code\
)\
icmd2status(ICMD_OP_GATTS_EXECUTE_WRITE_AUTHORIZE_CONFIRM, \
    U8(authorize), U8(error_code))

// ICMD_WAKELOCK (by WAKELOCK pin)

// ICMD_RESET,
#define RESET()\
    icmd2status(ICMD_OP_RESET)

// ICMD_AES128,
#define AES128(\
    p_key, p_plaintext_data, \
    p_encrypted_data\
)\
icmd2status(ICMD_OP_AES128, \
    PTR(p_key), PTR(p_plaintext_data), \
    PTR(p_encrypted_data))

// ICMD_DTM_ENABLE,
#define DTM_ENABLE(\
    enable\
)\
icmd2status(ICMD_OP_DTM_ENABLE, \
    U8(enable))

// ICMD_IEVT_MASK,
#define IEVT_MASK(\
    ievt_mask\
)\
icmd2status(ICMD_OP_IEVT_MASK, \
    U16(ievt_mask), U16(0))

// ICMD_ECHO_TEST,
#define ECHO_TEST(\
    length, p_data, \
    return_length, p_return_data \
) \
icmd2status(ICMD_OP_ECHO_TEST,\
    U8(length), VPTR(p_data), \
    PTR(return_length), VPTR(p_return_data))
    
// ICMD_GEN_RAND_NUMBER,
#define GEN_RAND_NUMBER(\
    \
    p_random_num\
)\
icmd2status(ICMD_OP_GEN_RAND_NUMBER, \
    \
    PTR(p_random_num))

// ICMD_EFUSE_WRITE (implemented in icmd_efuse.h)
// ICMD_EFUSE_READ  (implemented in icmd_efuse.h)

// ICMD_OP_RESERVED41,
// ICMD_OP_RESERVED42,

// ICMD_TEST_RECEIVER,
#define TEST_RECEIVER(\
    rx_frequency\
)\
icmd2status(ICMD_OP_TEST_RECEIVER, \
    U8(rx_frequency))

// ICMD_TEST_TRANSMITTER,
#define TEST_TRANSMITTER(\
    tx_frequency, num_of_packets, length, payload\
)\
icmd2status(ICMD_OP_TEST_TRANSMITTER, \
    U8(tx_frequency), U16(num_of_packets), U8(length), U8(payload))

// ICMD_TEST_END,
#define TEST_END(\
    \
    p_num_of_packets\
)\
icmd2status(ICMD_OP_TEST_END, \
    \
    PTR(p_num_of_packets))

// ICMD_ANCS_SUPPORT,
#define ANCS_SUPPORT(\
    enable\
)\
icmd2status(ICMD_OP_ANCS_SUPPORT, \
    U8(enable))

// ICMD_ANCS_DISCOVER,
#define ANCS_DISCOVER(\
)\
icmd2status(ICMD_OP_ANCS_DISCOVER)

// ICMD_ANCS_ENABLE_SOURCE,
#define ANCS_ENABLE_SOURCE(\
    type, enable\
)\
icmd2status(ICMD_OP_ANCS_ENABLE_SOURCE, \
    U8(type), U8(enable))

// ICMD_ANCS_ACTION,
#define ANCS_ACTION(\
    length, p_data \
) \
icmd2status(ICMD_OP_ANCS_ACTION, \
    U8(length), VPTR(p_data))

/**
 * global error code
 *
 */
extern uint8_t g_icmd_error_code;
#define ERROR_CODE (g_icmd_error_code)

typedef enum {
    ICMD_ERROR_INVALID_PARAM = 0xc0,
    ICMD_ERROR_INVALID_PARAM_LEN,
    ICMD_ERROR_INVALID_OPCODE,
    ICMD_ERROR_INVALID_CONNECTION,
    ICMD_ERROR_INVALID_IDEX,
    ICMD_ERROR_ADD_INCLUDE_SERVICE_NOT_EXIST,
    ICMD_ERROR_ADD_DESCRIPTOR_CHARACTERISTIC_NOT_EXIST,
    ICMD_ERROR_ADD_DESCRIPTOR_EXIST,
    ICMD_ERROR_CLIENT_DESCRIPTOR_NOT_EXIST,
    ICMD_ERROR_CLIENT_DESCRIPTOR_NOT_ALLOWED_INDICATION,
    ICMD_ERROR_INVALID_QUERY
}   ICMD_ERROR;

/*! BLE device address type */
typedef enum {
    ICMD_GAP_ADDR_TYPE_PUBLIC = 0x00,
    ICMD_GAP_ADDR_TYPE_RANDOM
}   ICMD_GAP_ADDR_TYPE;

/*! BDADDR */
#define BDADDR_LEN 6                    /*!< length of BLE device address */
typedef uint8_t bdaddr[BDADDR_LEN];     /*!< BLE device address */

/*! advertising type */
typedef enum {
    ICMD_GAP_ADV_TYPE_ADV_IND = 0x00,
    ICMD_GAP_ADV_TYPE_ADV_DIRECT_IND,
    ICMD_GAP_ADV_TYPE_ADV_SCAN_IND,  
    ICMD_GAP_ADV_TYPE_ADV_NONCONN_IND,
    ICMD_GAP_ADV_TYPE_ADV_SCAN_RSP
}   ICMD_GAP_ADV_TYPE;

// # permission
#define ICMD_GATTS_PERMISSION_READ                              (0x01)
#define ICMD_GATTS_PERMISSION_READ_AUTHENTICATED_MODE1_LEVEL2   (0x01 << 2)
#define ICMD_GATTS_PERMISSION_READ_AUTHENTICATED_MODE1_LEVEL3   (0x01 << 3)
#define ICMD_GATTS_PERMISSION_READ_AUTHORIZE                    (0x01 << 6)

#define ICMD_GATTS_PERMISSION_WRITE                             (0x01 << 1)
#define ICMD_GATTS_PERMISSION_WRITE_AUTHENTICATED_MODE1_LEVEL2  (0x01 << 4)
#define ICMD_GATTS_PERMISSION_WRITE_AUTHENTICATED_MODE1_LEVEL3  (0x01 << 5)
#define ICMD_GATTS_PERMISSION_WRITE_AUTHORIZE                   (0x01 << 7)

/**
 * iEVT part:
 *
 */
// iEVT OP:
typedef enum {
    IEVT_OP_NOP = 0x00,
    IEVT_OP_STATUS,
    IEVT_OP_GAP_DEVICE_READY = 0x02,
    IEVT_OP_GAP_CONNECTION_CMP,
    IEVT_OP_GAP_ADV_REPORT,
    IEVT_OP_GAP_CONNECTION_PARAM_UPDATE_RESPONSE,
    IEVT_OP_GAP_CONNECTION_UPDATE_CMP,
    IEVT_OP_GAP_DISCONNECT_CMP,
    IEVT_OP_GAP_TIMEOUT,                            // 0x08
    IEVT_OP_GAP_SECURITY_STATUS,
    IEVT_OP_GAP_SECURITY_TK_REQUEST,
    IEVT_OP_GAP_SECURITY_INFO_UPDATE,
    IEVT_OP_GATTS_SERVER_INDICATION_CONFIRM,
    IEVT_OP_GATTS_WRITE_SYNC,
    IEVT_OP_GATTS_WRITE_QUERY_AUTHORIZE,
    IEVT_OP_GATTS_READ_QUERY_AUTHORIZE,
    IEVT_OP_GATTS_PREPARE_WRITE_QUERY_AUTHORIZE,    //0x10
    IEVT_OP_GATTS_EXECUTE_WRITE_QUERY_AUTHORIZE,
    IEVT_OP_HARDFAULT,
    IEVT_OP_RESERVED13,
    IEVT_OP_ANCS_CMD_CMP,
    IEVT_OP_ANCS_DATA,
    IEVT_OP_GAP_METADATA_UPDATE,
    IEVT_OP_UNDEF
}   IEVT_OP;

// iEVT structure
/*! iCOMM-BLE version information*/
#define DEVICE_VERSION_SIZE (2)
typedef struct {
    uint8_t status;
    uint8_t version[DEVICE_VERSION_SIZE];
} PACKED ievt_param_gap_device_ready_st;
    
/*! iCMD GAP Role defination*/
typedef enum {
    ICMD_GAP_ROLE_TYPE_MASTER=0x00,
    ICMD_GAP_ROLE_TYPE_SLAVE
}   ICMD_GAP_ROLE_TYPE;

/*! GAP connection complete ievt*/
typedef struct {
    uint8_t status;
    //
    ICMD_GAP_ROLE_TYPE  role;
    ICMD_GAP_ADDR_TYPE  peer_addr_type;
    bdaddr              peer_addr;
    //
    uint8_t conn_interval[2];
    uint8_t conn_latency[2];
    uint8_t supervision_timeout[2];
    uint8_t master_clock_accuracy;
} PACKED ievt_param_gap_connection_cmp_st;

/*! GAP advertising report ievt*/
#define ICMD_GAP_ADV_DATA_LEN_MAX 31
typedef struct {
    ICMD_GAP_ADV_TYPE   event_type;
    ICMD_GAP_ADDR_TYPE  peer_addr_type;
    bdaddr              peer_addr;
    //
    uint8_t rssi;
    uint8_t adv_data_len;
    uint8_t adv_data[ICMD_GAP_ADV_DATA_LEN_MAX ];
} PACKED ievt_param_gap_adv_report_st;

/*! connection parameter update result defination*/
typedef enum {
    IEVT_GAP_CONNECTION_PARAM_UPDATE_RESPONSE_RESULT_ACCEPTED = 0x0000,
    IEVT_GAP_CONNECTION_PARAM_UPDATE_RESPONSE_RESULT_REJECTED,
    IEVT_GAP_CONNECTION_PARAM_UPDATE_RESPONSE_RESULT_TIMEOUT
}   IEVT_GAP_CONNECTION_PARAM_UPDATE_RESPONSE_RESULT;

/*! GAP connection parameter update response ievt*/
typedef struct {
    uint8_t result[2];
} PACKED ievt_param_gap_connection_param_update_response_st;

/*! GAP connection parameter update complete ievt*/
typedef struct {
    uint8_t status;
    uint8_t conn_interval[2];
    uint8_t conn_latency[2];
    uint8_t supervision_timeout[2];
} PACKED ievt_param_gap_connection_update_cmp_st;

/*! GAP disconnection complete ievt*/
typedef struct {
    uint8_t status;
    uint8_t reason;
} PACKED ievt_param_gap_disconn_cmp_st;

/*! GAP timeout type defination*/
typedef enum {
    IEVT_GAP_TIMEOUT_TYPE_ADV = 0x01,
    IEVT_GAP_TIMEOUT_TYPE_SCAN
}   IEVT_GAP_TIMEOUT_TYPE;

/*! GAP timeout ievt*/
typedef struct {
    IEVT_GAP_TIMEOUT_TYPE type;
} PACKED ievt_param_gap_timeout_st;

/*! GAP Security Status Result defination*/
typedef enum {
    IEVT_GAP_SEC_STATUS_RESULT_ENCRYPTION_WITH_NO_PAIR = 0x01,
    IEVT_GAP_SEC_STATUS_RESULT_ENCRYPTION_WITH_PAIR
}   IEVT_GAP_SEC_STATUS_RESULT;

/*! GAP Security Status Result duaring SM-pairing*/
typedef enum {
    IEVT_GAP_SEC_STATUS_PAIR_FAILED_REASON_RESERVED = 0x00,
    IEVT_GAP_SEC_STATUS_PAIR_FAILED_REASON_PASSKEY_ENTRY_FAILED,
    IEVT_GAP_SEC_STATUS_PAIR_FAILED_REASON_OOB_NOT_AVAILABLE,
    IEVT_GAP_SEC_STATUS_PAIR_FAILED_REASON_AUTHENT_REQUIRE,
    IEVT_GAP_SEC_STATUS_PAIR_FAILED_REASON_CONFIRM_VALUE_FAILED,
    IEVT_GAP_SEC_STATUS_PAIR_FAILED_REASON_PAIR_NOT_SUPPORT,
    IEVT_GAP_SEC_STATUS_PAIR_FAILED_REASON_ENCRY_KEY_SIZE,
    IEVT_GAP_SEC_STATUS_PAIR_FAILED_REASON_COMM_NOT_SUPPORT,
    IEVT_GAP_SEC_STATUS_PAIR_FAILED_REASON_UNSPEC_REASON,
    IEVT_GAP_SEC_STATUS_PAIR_FAILED_REASON_REPEAT_ATTEMPT,
    IEVT_GAP_SEC_STATUS_PAIR_FAILED_REASON_INVALID_PARAM
}   IEVT_GAP_SEC_STATUS_PAIR_FAILED_REASON;

/*! GAP security status ievt*/
typedef struct {
    uint8_t status;
    //
    IEVT_GAP_SEC_STATUS_RESULT              result;
    IEVT_GAP_SEC_STATUS_PAIR_FAILED_REASON  reason;
} PACKED ievt_param_gap_security_status_st;

/*! SM TK request type (passkey input/display/OOB input) */
typedef enum {
    IEVT_GAP_SEC_TK_REQUEST_TYPE_PASSKEY_INPUT = 0x00,
    IEVT_GAP_SEC_TK_REQUEST_TYPE_PASSKEY_DISPLAY,
    IEVT_GAP_SEC_TK_REQUEST_TYPE_OOB_INPUT
}   IEVT_GAP_SEC_TK_REQUEST_TYPE;

/*! GAP security TK request ievt*/
typedef struct {
    IEVT_GAP_SEC_TK_REQUEST_TYPE type;
} PACKED ievt_param_gap_security_tk_request_st;


/*! type of security info update*/
typedef enum {
    IEVT_GAP_SEC_INFO_UPDATE_TYPE_PEER_ADDR = 0x00,
    IEVT_GAP_SEC_INFO_UPDATE_TYPE_PEER_LTK,
    IEVT_GAP_SEC_INFO_UPDATE_TYPE_PEER_EDIV_RAND64,
    IEVT_GAP_SEC_INFO_UPDATE_TYPE_PEER_IRK,
    IEVT_GAP_SEC_INFO_UPDATE_TYPE_PEER_CSRK
}   IEVT_GAP_SEC_INFO_UPDATE_TYPE;

/*! GAP security information update ievt*/
#define IEVT_GAP_SEC_INFO_UPDATE_TYPE_PEER_ADDR_LEN         (0x07)
#define IEVT_GAP_SEC_INFO_UPDATE_TYPE_PEER_EDIV_RAND64_LEN  (0x0a)
#define ICMD_SEC_KEY_SIZE                                   (16)
typedef struct {
    IEVT_GAP_SEC_INFO_UPDATE_TYPE type;
    //
    uint8_t key_len;
    uint8_t key[ICMD_SEC_KEY_SIZE];
} PACKED ievt_param_gap_security_info_update_st;

/*! GATTS server indicate comfirm ievt*/
typedef struct {
    uint8_t index;
    uint8_t status;
} PACKED ievt_param_gatts_server_indication_confirm_st;

/*! maximum value size of GATTS characteristic stored in ICOMM_BLE*/
#define IEVT_GATTS_VALUE_SIZE_MAX 22

/*! GATTS write sync ievt*/
typedef struct {
    uint8_t index;
    uint8_t value_size;
    uint8_t value[IEVT_GATTS_VALUE_SIZE_MAX];
} PACKED ievt_param_gatts_write_sync_st;

/*! GATTS write query authorize ievt*/
typedef struct {
    uint8_t index;
    uint8_t value_size;
    uint8_t value[IEVT_GATTS_VALUE_SIZE_MAX];
} PACKED ievt_param_gatts_write_query_authorize_st;

/*! GATTS read query authorize ievt*/
typedef struct {
    uint8_t index;
    uint8_t value_offset[2];
} PACKED ievt_param_gatts_read_query_authorize_st;

/*! GATTS prepare write query authorize ievt*/
typedef struct {
    uint8_t index;
    uint8_t value_offset[2];
    uint8_t value_size;
    uint8_t value[IEVT_GATTS_VALUE_SIZE_MAX];
} PACKED ievt_param_gatts_prepare_write_query_authorize_st;

/*! GATTS execute write query authorize ievt*/
typedef enum {
    IEVT_GATTS_EXECUTE_WRITE_FLAG_CANCELED = 0x00,
    IEVT_GATTS_EXECUTE_WRITE_FLAG_EXECUTE
}   IEVT_GATTS_EXECUTE_WRITE_FLAG;

typedef struct {
    IEVT_GATTS_EXECUTE_WRITE_FLAG execute_flag;
} PACKED ievt_param_gatts_execute_write_query_authorize_st;

/*! SYSTEM hardfault ievt */
typedef struct {
    uint8_t hardfault_code;
    uint8_t description[4];
} PACKED ievt_param_system_hardfault_st;

 /*! ANCS command types */
 typedef enum {
     IEVT_ANCS_DISCOVER_CMD = 0x00,
     IEVT_ANCS_ENABLE_NOTIF_SOURCE_CMD,
     IEVT_ANCS_ENABLE_DATA_SOURCE_CMD,
     IEVT_ANCS_ACTION_CMD
 } IEVT_ANCS_CMD_TYPE;

 /*! ANCS command completed ievt */
 typedef struct {
     IEVT_ANCS_CMD_TYPE cmd;
     uint8_t status;
     uint8_t err_reason;
 } PACKED ievt_param_ancs_cmd_cmp_st;

 /*! ANCS event types */
 typedef enum {
     IEVT_ANCS_NOTIF_SOURCE_EVT = 0x00,
     IEVT_ANCS_DATA_SOURCE_EVT
 } IEVT_ANCS_EVT_TYPE;

 /*! ANCS data ievt */
 typedef struct {
     IEVT_ANCS_EVT_TYPE type;
     uint8_t value_size;
     uint8_t value[IEVT_GATTS_VALUE_SIZE_MAX];
 } PACKED ievt_param_ancs_data_st;

#define ICMD_GAP_METADATA_LENGTH_MAX   2
 /*! GAP metadata update data ievt */
 typedef struct {
     uint8_t value_size;
     uint8_t value[ICMD_GAP_METADATA_LENGTH_MAX];
 } PACKED ievt_param_gap_metadata_update_st;

 /*! defines the maximum length of ievt iCOMM-BLE send*/
/** 
 * parameter size of each event
    ievt_param_gap_device_ready_st 0x03
    ievt_param_gap_connection_cmp_st 0x10
    ievt_param_gap_adv_report_st 0x29
    ievt_param_gap_connection_param_update_response_st 0x02
    ievt_param_gap_connection_update_cmp_st 0x07
    ievt_param_gap_disconn_cmp_st 0x02
    ievt_param_gap_timeout_st 0x01
    ievt_param_gap_security_status_st 0x03
    ievt_param_gap_security_tk_request_st 0x01
    ievt_param_gap_security_info_update_st 0x12
    ievt_param_gatts_server_indication_confirm_st 0x02
    ievt_param_gatts_write_sync_st 0x18
    ievt_param_gatts_write_query_authorize_st 0x18
    ievt_param_gatts_read_query_authorize_st 0x03
    ievt_param_gatts_prepare_write_query_authorize_st 0x1a
    ievt_param_gatts_execute_write_query_authorize_st 0x01
    ievt_param_system_hardfault_st 0x05
    ievt_param_ancs_cmd_cmp_st 0x03
    ievt_param_ancs_data_st 0x18
    ievt_param_gap_metadata_update_st 0x03
*/


#define IEVT_PARAM_LEN_MAX 24


/**
 * iEVT parameter union.
 */
typedef union {
    ievt_param_gap_device_ready_st                      gap_device_ready;
    ievt_param_gap_connection_cmp_st                    gap_connection_cmp;
    ievt_param_gap_adv_report_st                        gap_adv_report;
    ievt_param_gap_connection_param_update_response_st  gap_connection_param_update_response;
    ievt_param_gap_connection_update_cmp_st             gap_connection_update_cmp;
    ievt_param_gap_disconn_cmp_st                       gap_disconn_cmp;
    ievt_param_gap_timeout_st                           gap_timeout;
    ievt_param_gap_security_status_st                   gap_security_status;
    ievt_param_gap_security_tk_request_st               gap_security_tk_request;
    ievt_param_gap_security_info_update_st              gap_security_info_update;
    ievt_param_gatts_server_indication_confirm_st       gatts_server_indication_confirm;
    ievt_param_gatts_write_sync_st                      gatts_write_sync;
    ievt_param_gatts_write_query_authorize_st           gatts_write_query_authorize;
    ievt_param_gatts_read_query_authorize_st            gatts_read_query_authorize;
    ievt_param_gatts_prepare_write_query_authorize_st   gatts_prepare_write_query_authorize;
    ievt_param_gatts_execute_write_query_authorize_st   gatts_execute_write_query_authorize;
    ievt_param_system_hardfault_st                      system_hardfault;
    ievt_param_ancs_cmd_cmp_st                          ancs_cmd_cmp;
    ievt_param_ancs_data_st                             ancs_data;
    ievt_param_gap_metadata_update_st                   gap_metadata_update;
} PACKED ievt_param_t;

/**
 * macro to translate ievt param as structure
 *
 */
#define IEVT_PARAM_S(PARAM) ((ievt_param_t*)(PARAM))

typedef struct {
    
    // header
    uint8_t indicator;
    uint8_t op;
    uint8_t param_len;

    // param
    uint8_t param[IEVT_PARAM_LEN_MAX];

} PACKED ievt_t;

/**
 * wakelock
 *
 */
void    icmd_set_wakelock_init  (void);
int8_t  icmd_set_wakelock       (bool wakelock);

/**
 * ievt
 *
 */
extern int8_t g_ievt_count;
void ievt_count_decrement(void);

#define iEVT_PENDING() ((g_ievt_count > 0))

int8_t get_ievt(ievt_t* ievt);

#endif // _ICMD_API_H_
