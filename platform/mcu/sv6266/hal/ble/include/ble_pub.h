#ifndef _BLE_PUB_H_
#define _BLE_PUB_H_
#include "breeze_hal_ble.h"

#define BLE_SUCCESS			(0)
#define BLE_FAILURE			(1)
#define BLE_DEV_NAME		"ble"

typedef void(*txdone)(uint8_t res);
extern ais_bt_init_t ais_bt_init_info;
extern int ble_main(ais_bt_init_t *ais_init);
extern void ble_task(void *pdata);

extern void ble_deinit(void);
extern int8_t ble_string_notification_send(uint8_t index, uint8_t value_len, uint8_t* value);
extern int8_t ble_string_indication_send(uint8_t index, uint8_t value_len, uint8_t* value);
extern void ble_gap_disconnect_cmd(void);
extern void ble_icmd_gap_adv(int enable);
extern void convert_adv_data(ais_adv_init_t *adv_data);
extern on_char_write_t wc_handler;
extern on_char_write_t wwnrc_handler;


#endif
