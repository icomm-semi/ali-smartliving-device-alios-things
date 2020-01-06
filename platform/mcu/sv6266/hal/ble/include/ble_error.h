/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef _BLE_ERROR_H_
#define _BLE_ERROR_H_

#define BLE_ERROR_UNKNOWN_HCI_COMMAND                                       0x01
#define BLE_ERROR_UNKNOWN_CONNECTION_IDENTIFIER                             0x02
#define BLE_ERROR_HARDWARE_FAILURE                                          0x03
#define BLE_ERROR_PAGE_TIMEOUT                                              0x04
#define BLE_ERROR_AUTHENTICATION_FAILURE                                    0x05
#define BLE_ERROR_PIN_OR_KEY_MISSING                                        0x06
#define BLE_ERROR_MEMORY_CAPACITY_EXCEEDED                                  0x07
#define BLE_ERROR_CONNECTION_TIMEOUT                                        0x08
#define BLE_ERROR_CONNECTION_LIMIT_EXCEEDED                                 0x09
#define BLE_ERROR_SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED         0x0A
#define BLE_ERROR_ACL_CONNECTION_ALREADY_EXISTS                             0x0B
#define BLE_ERROR_COMMAND_DISALLOWED                                        0x0C
#define BLE_ERROR_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES              0x0D
#define BLE_ERROR_CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS               0x0E
#define BLE_ERROR_CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR           0x0F
#define BLE_ERROR_CONNECTION_ACCEPT_TIMEOUT_EXCEEDED                        0x10
#define BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE                    0x11
#define BLE_ERROR_INVALID_HCI_COMMAND_PARAMETERS                            0x12
#define BLE_ERROR_REMOTE_USER_TERMINATED_CONNECTION                         0x13
#define BLE_ERROR_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_LOW_RESOURCES  0x14
#define BLE_ERROR_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_POWER_OFF      0x15
#define BLE_ERROR_CONNECTION_TERMINATED_BY_LOCAL_HOST                       0x16
#define BLE_ERROR_REPEATED_ATTEMPTS                                         0x17
#define BLE_ERROR_PAIRING_NOT_ALLOWED                                       0x18
#define BLE_ERROR_UNKNOWN_LMP_PDU                                           0x19
#define BLE_ERROR_UNSUPPORTED_REMOTE_FEATURE_UNSUPPORTED_LMP_FEATURE        0x1A
#define BLE_ERROR_SCO_OFFSET_REJECTED                                       0x1B
#define BLE_ERROR_SCO_INTERVAL_REJECTED                                     0x1C
#define BLE_ERROR_SCO_AIR_MODE_REJECTED                                     0x1D
#define BLE_ERROR_INVALID_LMP_PARAMETERS                                    0x1E
#define BLE_ERROR_UNSPECIFIED_ERROR                                         0x1F
#define BLE_ERROR_UNSUPPORTED_LMP_PARAMETER_VALUE                           0x20
#define BLE_ERROR_ROLE_CHANGE_NOT_ALLOWED                                   0x21
#define BLE_ERROR_LMP_RESPONSE_TIMEOUT_LL_RESPONSE_TIMEOUT                  0x22
#define BLE_ERROR_LMP_ERROR_TRANSACTION_COLLISION                           0x23
#define BLE_ERROR_LMP_PDU_NOT_ALLOWED                                       0x24
#define BLE_ERROR_ENCRYPTION_MODE_NOT_ACCEPTABLE                            0x25
#define BLE_ERROR_LINK_KEY_CANNOT_BE_CHANGED                                0x26
#define BLE_ERROR_REQUESTED_QOS_NOT_SUPPORTED                               0x27
#define BLE_ERROR_INSTANT_PASSED                                            0x28
#define BLE_ERROR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED                       0x29
#define BLE_ERROR_DIFFERENT_TRANSACTION_COLLISION                           0x2A
#define BLE_ERROR_QOS_UNACCEPTABLE_PARAMETER                                0x2C
#define BLE_ERROR_QOS_REJECTED                                              0x2D
#define BLE_ERROR_CHANNEL_ASSESSMENT_NOT_SUPPORTED                          0x2E
#define BLE_ERROR_INSUFFICIENT_SECURITY                                     0x2F
#define BLE_ERROR_PARAMETER_OUT_OF_MANDATORY_RANGE                          0x30
#define BLE_ERROR_ROLE_SWITCH_PENDING                                       0x32
#define BLE_ERROR_RESERVED_SLOT_VIOLATION                                   0x34
#define BLE_ERROR_ROLE_SWITCH_FAILED                                        0x35
#define BLE_ERROR_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE                       0x36
#define BLE_ERROR_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST                      0x37
#define BLE_ERROR_HOST_BUSY_PAIRING                                         0x38
#define BLE_ERROR_CONNECTION_REJECTED_DUE_TO_NO_SUITABLE_CHANNEL_FOUND      0x39
#define BLE_ERROR_CONTROLLER_BUSY                                           0x3A
#define BLE_ERROR_UNACCEPTABLE_CONNECTION_INTERVAL                          0x3B
#define BLE_ERROR_DIRECTED_ADVERTISING_TIMEOUT                              0x3C
#define BLE_ERROR_CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE                  0x3D
#define BLE_ERROR_CONNECTION_FAILED_TO_BE_ESTABLISHED                       0x3E
#define BLE_ERROR_MAC_CONNECTION_FAILED                                     0x3F

#endif // _BLE_ERROR_H_
