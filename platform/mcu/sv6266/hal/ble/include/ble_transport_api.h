/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef _BLE_TRANSPORT_API_H_
#define _BLE_TRANSPORT_API_H_
#include "types.h"
#include "breeze_hal_ble.h"

/**
 * Load configurations of BLE Register Service, including:
 *  -Device Configuration
 *  -GATT Database
 *  -Advertising Data, Parameter, and Enable
 *
 */
void ble_transport_init(void);
void ble_transport_adv2connect_on_disconnect(void);



#define BLE_UUID_AIS_WC_INDEX 0x05
#define BLE_UUID_AIS_IC_INDEX 0x07
#define BLE_UUID_AIS_IC_DES_INDEX 0x08
#define BLE_UUID_AIS_WWNRC_INDEX 0x09
#define BLE_UUID_AIS_NC_INDEX 0x0b
#define BLE_UUID_AIS_NC_DES_INDEX 0x0c



int8_t ble_string_notification_send(uint8_t index, uint8_t value_len, uint8_t* value);
void ble_transport_string_received_on_write_sync(ievt_param_gatts_write_sync_st* ievt_param_gatts_write_sync);

// Please implement on_updated based on your application
extern connected_callback_t sv_ble_connected;
extern disconnected_callback_t sv_disconnected;
extern on_char_ccc_change_t ble_ic_ccc_change;
extern on_char_ccc_change_t ble_nc_ccc_change;
extern uint8_t ble_transport_device_name(uint8_t *name);


#endif // _BLE_TRANSPORT_API_H_
