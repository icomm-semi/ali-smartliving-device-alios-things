#include "ble_pub.h"
#include "AIS_pub.h"
#include "breeze_hal_ble.h"
#include <stddef.h>
#include <stdint.h>
#include <aos/aos.h>
#include "ali_crypto.h"



ais_err_t ble_stack_init(ais_bt_init_t *ais_init)
{
  return ble_main(ais_init);
}

ais_err_t ble_stack_deinit()
{
    ble_deinit();
    return AIS_ERR_SUCCESS;
}

ais_err_t ble_send_notification(uint8_t *p_data, uint16_t length)
{
	uint8_t i;

    ais_err_t status = AIS_ERR_SUCCESS;
    ble_string_notification_send(11, length, p_data);
    return status;
}

ais_err_t ble_send_indication(uint8_t *p_data, uint16_t length, void (*txdone)(uint8_t res))
{
	uint8_t i;
    ais_err_t status = AIS_ERR_SUCCESS;
    ble_string_indication_send(7, length, p_data);
	set_send_indication_cb(txdone);
    return status;
}

void ble_disconnect(uint8_t reason)
{
	ble_gap_disconnect_cmd();
}

ais_err_t ble_advertising_start(ais_adv_init_t *adv)
{

   ais_err_t status = AIS_ERR_SUCCESS;
   convert_adv_data(adv);
   
   ble_transport_device_name(adv->name.name);
   ble_icmd_gap_adv(1);
   return status;
}

ais_err_t ble_advertising_stop()
{
    ais_err_t status = AIS_ERR_SUCCESS;
    ble_icmd_gap_adv(0);
    return status;
}

ais_err_t ble_get_mac(uint8_t *mac)
{
	ais_err_t status = AIS_ERR_SUCCESS;
	get_ble_mac_addr(mac);
    return status;
}


