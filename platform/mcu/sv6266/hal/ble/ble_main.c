/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#include "types.h"
#include <string.h>
#include "AIS_pub.h"
#include "icmd_api.h"
#include "ble_error.h"
#include "icmd.h"
#include "icmd_interface.h"
#include "icmd_interface_hal.h"
#include "icmd2load.h"
#include "icmd_patch.h"
#include "terminal.h"
#include "ble_bsp.h"
#include "osal.h"
#include "ble_transport_api.h"
#include "ble_transport_api_external.h"
#include "ble_pub.h"
#include "gpio/hal_gpio.h"
#include "ble_pub.h"

static uint8_t g_status_code = 0;
static bool isconnected = false;
static bool ble_task_running = false;

extern uint8_t al_stack_init(ais_bt_init_t *ais_init);

typedef void (*ble_txdone)(uint8_t res);

enum {
    BSP_INDICATION_BLE_READY = 0,
    BSP_INDICATION_BLE_TRANSPORT_READY,
    BSP_INDICATION_CONNECTED,
    BSP_INDICATION_DISCONNECTED,
    //
    BSP_INDICATION_EXCEPTION = 255
};

/**
 * application program
 *
 */
static void device_on_ievt(ievt_t* ievt) {
    switch (ievt->op) {
        case IEVT_OP_HARDFAULT:
        case IEVT_OP_GAP_DEVICE_READY:
            // log
            ERROR_LOG_STR("\n! BLE device hardfault:");
            ERROR_LOG_ARRAY(ievt->param, ievt->param_len);
            ERROR_CHECK(BLE_ERROR_HARDWARE_FAILURE);
            break;
    }
}
ble_txdone g_txdone = NULL;
void ble_transport_gatts_server_indication_confirm (ievt_param_gatts_server_indication_confirm_st*  gatts_server_indication_confirm)
{
	if(g_txdone != NULL)
    	g_txdone(gatts_server_indication_confirm->status);
}

static void ble_transport_on_ievt(ievt_t* ievt) {
    switch (ievt->op) {
        case IEVT_OP_GAP_CONNECTION_CMP:
        	isconnected = true;
            bsp_indication(BSP_INDICATION_CONNECTED);
            sv_ble_connected();
            break;
        case IEVT_OP_GAP_DISCONNECT_CMP:
        	isconnected = false;
            bsp_indication(BSP_INDICATION_DISCONNECTED);
            ble_transport_adv2connect_on_disconnect();
            sv_disconnected();
            break;
        case IEVT_OP_GATTS_SERVER_INDICATION_CONFIRM:
         	ble_transport_gatts_server_indication_confirm((void*)&ievt->param);
            break;    
        case IEVT_OP_GATTS_WRITE_SYNC:
            ble_transport_string_received_on_write_sync ((void*)&ievt->param);
            break;
    }
}




// ievt storage
ievt_t XDATA g_ievt;
static void on_ievt(ievt_t* ievt) {

    if (ievt->op == IEVT_OP_NOP) {
        return;
    }
    TERMINAL_STR("\n# got evt: "); TERMINAL_HEX(ievt->op);
    
    // mandatory ievt handler
    device_on_ievt(ievt);

    // service on evt
    ble_transport_on_ievt(ievt);
}

static int ble_init(void) {

	int retry_cnt = 0;
    // # BLE HW Reset
//wait_ble_boot:
    icmd_interface_rstn();
    OS_MsDelay(10);

    // initial BLE wakelock interface
    icmd_set_wakelock_init();
wait_ble_boot:
    // wait BLE boot
    while(icmd_interface_irq() != IEVT_IRQ_ACTIVE)
	{
		retry_cnt++;
		if(retry_cnt > 10)
		{
			printf("Wait for ble ready timeout\n");
			goto exception;
		}
		OS_MsDelay(100);
	}

    // BLE patch
    // check FW version before patch
    if (get_ievt(&g_ievt) != 0) {
        goto exception;
    }
    // A GAP_DEVICE_READY is GUARANTEED. In SPI, due to asynchronous nature, some pending STATUS/iEVT may be recevied here, after RESET CMD.
    if (g_ievt.op != IEVT_OP_GAP_DEVICE_READY) {
        goto exception;
    }
    TERMINAL_STR("\n# BLE device version:");
    TERMINAL_ARRAY(IEVT_PARAM_S(g_ievt.param)->gap_device_ready.version, DEVICE_VERSION_SIZE);

    // load patch, if base version match
    if (U8_TO_U16(IEVT_PARAM_S(g_ievt.param)->gap_device_ready.version) == icmd_patch_base_version) {
        TERMINAL_STR("\n# apply BLE patch...");
        if (icmd_tbl2load(&icmd_patch[0], icmd_patch_size) != icmd_patch_size) {
            goto exception;
        }
       goto  wait_ble_boot;
    }
    return 0;
    
exception:
    ERROR_LOG_STR("\n! ble_init exception");
    ERROR_CHECK(0xff);
	return 1;
}

void status_handler() {

    uint8_t status_code = g_status.param[STATUS_PARAM_STATUS_CODE_INDEX];
	g_status_code = status_code;
    if(status_code == 0) {
        return;
    }

    if (g_status.param[STATUS_PARAM_ICMD_OP_INDEX] == ICMD_OP_GATTS_SERVER_INDICATE) {
        switch (status_code) {
            case 0xc9:  // ICMD_ERROR_CLIENT_DESCRIPTOR_NOT_ALLOWED_INDICAT
                ERROR_LOG_STR("\n! ERROR: CCCD not allow indication");
                /* fall through */
            case 0x84:  // ERR_HOST_INSUFF_RESOURCE
                // BLE doesn't have sufficient tx buffer to hold indication/notification
                /* fall through */
            case 0xc3:  // ICMD_ERROR_INVALID_CONNECTION
                // BLE lose connection
                printf("ble los connection\n");
                return;
            default:
                goto unexpected_error;
        }
    }

unexpected_error:
    ERROR_LOG_STR("\n! STATUS PARAM:");
    ERROR_LOG_ARRAY(g_status.param, STATUS_PARAM_LEN_MAX);
    bsp_indication(0xff);

    //ERROR_CHECK(status_code);
}





void set_send_indication_cb(ble_txdone res)
{
	g_txdone = res;
}
void ble_task(void *pdata)                                                                                                                                  
{
    while(ble_task_running)
    {
        if(iEVT_PENDING())
		{
            if(get_ievt(&g_ievt) == 0) {
                on_ievt(&g_ievt);
            }
        }
		OS_MsDelay(20);
        poll_ble_ievt();
    }
	OS_TaskDelete(NULL);
}

int ble_main(ais_bt_init_t *ais_init)
{	
    // # mcu/board depedent initialzations
	//smart_init();
	int ble_init_ret;
	

    ble_bsp_init();

	

    // initial BLE
    ble_init_ret = ble_init();
	if(ble_init_ret != 0)
		return ble_init_ret;
   
    bsp_indication(BSP_INDICATION_BLE_READY);
	
    // initial BLE Transport
    ble_transport_init();
	al_stack_init(ais_init);
    
    bsp_irq_enable();
    bsp_indication(BSP_INDICATION_BLE_TRANSPORT_READY);
	ble_task_running = true;
    OS_TaskCreate(ble_task, "ble_task", 1024, NULL, OS_TASK_LOW_PRIO, NULL);
}

void ble_deinit(void)
{
	bsp_irq_disable();
	drv_gpio_set_logic(BLE_RESET_IO, GPIO_LOGIC_LOW);
	ble_task_running = false;
}


