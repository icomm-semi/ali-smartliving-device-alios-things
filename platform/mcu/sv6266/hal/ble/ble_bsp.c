/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include "ble_bsp.h"
#include "ievt_handler.h"
#include "icmd_interface.h"
#include "icmd_interface_hal.h"
#include "terminal.h"
#include "gpio/hal_gpio.h"
#include "gpio/drv_gpio.h"
#include "tmr/drv_tmr.h"
#include "soc_defs.h"
#include "icmd.h"
#include "spimst/drv_spimst.h"
#include "pinmux/hal_pinmux.h"
uint8_t tmr_irq;


static void tmr_irq_handler0 (uint32_t irq_num)
{
   	
    // # do query
    drv_tmr_clear_interrupt_status (irq_num - IRQ_UTIMER_0);
    //TERMINAL_STR("\n# tmr_irq_handler0sts: ");
    tmr_irq=1;
}


void ble_bsp_init() {
	uint8_t rets;
    TERMINAL_STR("\n ble_bsp_init\n");
  	//reset GPIO11
	rets = drv_gpio_set_mode(BLE_RESET_IO, PIN_MODE_GPIO);
	rets = drv_gpio_set_pull(BLE_RESET_IO, GPIO_PULL_UP);
	rets = drv_gpio_set_dir(BLE_RESET_IO, GPIO_DIR_OUT);
	rets = drv_gpio_set_logic(BLE_RESET_IO, GPIO_LOGIC_LOW);
#if 1
	//wakelock GPIO09   P23, WAKELOCK
	rets = drv_gpio_set_mode(BLE_WAKELOCK_IO, PIN_MODE_GPIO);
	rets = drv_gpio_set_pull(BLE_WAKELOCK_IO, GPIO_PULL_UP);
	rets = drv_gpio_set_dir(BLE_WAKELOCK_IO, GPIO_DIR_OUT);
	rets = drv_gpio_set_logic(BLE_WAKELOCK_IO, GPIO_LOGIC_HIGH);
#endif	
	//spi 模式选择脚.在没开机初始化前先置1
	drv_pinmux_manual_function_select_disable(SEL_SPI_MST);
	rets = drv_gpio_set_mode(GPIO_10, PIN_MODE_GPIO);
	rets = drv_gpio_set_pull(GPIO_10, GPIO_PULL_UP);
	rets = drv_gpio_set_dir(GPIO_10, GPIO_DIR_OUT);
	rets = drv_gpio_set_logic(GPIO_10, GPIO_LOGIC_HIGH);
	
	
    // BSP indication
    terminal_init();
    
    
    
//	rets = drv_tmr_init (TM_TM2_MS);
//    rets = drv_tmr_sw_rst (TM_TM2_MS);
//    rets = drv_tmr_register_irq_handler (TM_TM2_MS, tmr_irq_handler0);
    //rets = drv_tmr_enable (TM_TM2_MS, 1,  200);
 
}
void poll_ble_ievt() {
	uint8_t sts;
	uint8_t status_length;

//	if(tmr_irq)
//	{
//		tmr_irq =  0;
//		OS_MsDelay(1);
	  	icmd_interface_poll_spim_trx(POLL_INDICATOR, &sts, &status_length);
	    //TERMINAL_STR("\n# tmr_irq ");
	    
	    if((sts & 0x04) == 0x04) {
	        ERROR_LOG_STR("\n! [ievt query]SPI STS TRX error: "); ERROR_LOG_HEX(sts);
	        return;
	    }
	    
	    if((sts & 0x03) == 0x03) {
	   	
	        ievt_irq_handler();
	    } 
//    }
}


void bsp_irq_enable() {
//    int ret;
//	ret = drv_tmr_enable (TM_TM2_MS, 1,  10);
}
void bsp_irq_disable() {
//	int ret;
//	ret=drv_tmr_disable(TM_TM2_MS);
}
void bsp_wdt_clear() {
                      
}

/**
 * On Board Indication
 *
 */

void bsp_indication(uint8_t indication) {
    terminal_str("\n# BSP indication:"); terminal_hex(indication);
}
