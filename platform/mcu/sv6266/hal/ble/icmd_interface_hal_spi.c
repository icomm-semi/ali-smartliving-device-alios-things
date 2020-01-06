/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
/**
 * iCMD interface implementation
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "types.h"
#include "drv_spimst.h"
#include "icmd_interface_hal.h"
#include "terminal.h"
#include "gpio/hal_gpio.h"
#include "gpio/drv_gpio.h"
#include "pinmux/hal_pinmux.h"

//#include "sys/bsp.h"
#include "osal.h"

#include "icmd.h"
#include "ble_bsp.h"


uint8_t *ble_rx_buf;
uint8_t *ble_tx_buf;

/**
 * physical interface implementation
 *
 */
void icmd_interface_spim_tx(uint8_t raw) {
    ble_tx_buf[0]=raw;
    drv_spimst_dma_trx(ble_tx_buf, ble_rx_buf, 1, BLE_SPI_CS_IO);
}

void icmd_interface_spim_tx_buf(uint8_t* raw,uint8_t len) {
    memcpy(ble_tx_buf, raw, len);
    drv_spimst_dma_trx(ble_tx_buf, ble_rx_buf, len, BLE_SPI_CS_IO);
    //TERMINAL_STR("\n# ble_tx_buf: "); TERMINAL_ARRAY(ble_tx_buf,len);
}

void icmd_interface_poll_spim_trx(uint8_t tx, uint8_t* rx, uint8_t* rx_length) {	
	ble_tx_buf[0]=tx;
	ble_tx_buf[1]=0;
	memset(ble_rx_buf, 0x00, sizeof( ble_rx_buf ));
	
    drv_spimst_dma_trx(ble_tx_buf, ble_rx_buf, 2, BLE_SPI_CS_IO);
    *rx = ble_rx_buf[0];
  	*rx_length = ble_rx_buf[1];
  	OS_MsDelay(1);
  	//TERMINAL_STR("\nread_buf[0]: "); TERMINAL_HEX(ble_rx_buf[0]);
}
void icmd_interface_query_spim_trx(uint8_t tx, uint8_t* rx, uint8_t tx_length) {
	uint32_t l;
	l = tx_length + 1;
	memset(ble_tx_buf, 0x00, sizeof( ble_tx_buf ));
	memset(ble_rx_buf, 0x00, sizeof( ble_rx_buf ));
	ble_tx_buf[0] = tx;
    drv_spimst_dma_trx(ble_tx_buf, ble_rx_buf, l, BLE_SPI_CS_IO);
    memcpy(rx, ble_rx_buf, l);
}


/**
 * ICMD interface operation
 *
 */
#define DELAY_1US \
do{\
    volatile int i = 0;\
    i++;\
}while(0)


/**
 * WAKELOCK (P2.1)
 *  set WAKELOCK pin level of ICMD
 *
 */
void icmd_interface_wakelock(bool wakelock) {

//30ms extra delay is needed to wait for 7050Q settle down.
#define WAKELOCK_DOWN_SETTLE_TIME (30000)

    TERMINAL_STR("\n# WAKELOCK:"); TERMINAL_HEX(wakelock);
    if(wakelock){
		drv_gpio_set_logic(BLE_WAKELOCK_IO, GPIO_LOGIC_HIGH);
	}
	else{
		drv_gpio_set_logic(BLE_WAKELOCK_IO, GPIO_LOGIC_LOW);
        OS_UsDelay(30);//system_delay(30);
	}
}

/**
 *  check IRQ level of ICMD
 *
 */
bool icmd_interface_irq() {
	uint8_t sts;
	uint8_t status_length = 0;
	
	icmd_interface_poll_spim_trx(POLL_INDICATOR, &sts, &status_length);
    if((sts & 0x04) == 0x04) {
        ERROR_LOG_STR("\n! icmd_interface_irq error: "); ERROR_LOG_HEX(sts);
    }
    
	if((sts & 0x03) == 0x03)
	    return 0;
	    
    return 1;
}

/**
 * RSTN
 *  - control HW_RSTN
 *
 */
#if 0 
#define DELAY_10US \
do{\
    volatile int i = 0;\
    for(i = 0; i < 65210; i++) {}\
}while(0)
#endif
#define DELAY_10US OS_MsDelay(1)


void icmd_interface_rstn() {
	drv_gpio_set_logic(BLE_RESET_IO, GPIO_LOGIC_HIGH);
	DELAY_10US;
    drv_gpio_set_logic(BLE_RESET_IO, GPIO_LOGIC_LOW);
	DELAY_10US;
	drv_gpio_set_logic(BLE_RESET_IO, GPIO_LOGIC_HIGH);
	DELAY_10US;
	hal_pinmux_manual_function_select_enable(SEL_SPI_MST);
	drv_spimst_init(5000000, 0, 0);
 	ble_rx_buf = drv_spimst_alloc_dma_buffer(70);
    ble_tx_buf = drv_spimst_alloc_dma_buffer(70);
    drv_spimst_set_dma_buffer(70, ble_tx_buf, ble_rx_buf);
}
