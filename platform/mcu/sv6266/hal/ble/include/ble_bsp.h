/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/**
 * BSP prototypes, SHALL be implemented on different platform.
 *
 */
#ifndef _BSP_H_
#define _BSP_H_

#include "types.h"

/**
 * Application dependent implementation
 *
 */
void ble_bsp_init(void);
void poll_ble_ievt(void);




#define BLE_RESET_IO	GPIO_05 	
#define BLE_WAKELOCK_IO	GPIO_02
#define BLE_SPI_CS_IO	GPIO_13

/**
 * critical section implementation
 *
 */
void bsp_irq_enable (void);
void bsp_irq_disable(void);

/**
 * WARNING: DO NOT adopt ERROR_CHECK in MP project
 *
 */
// #define ERROR_CHECK(ERROR_NO)
#define ERROR_CHECK(ERROR_NO) \
do { \
    if(ERROR_NO){ \
        ERROR_LOG_STR("\n! Error: "); \
        ERROR_LOG_HEX(ERROR_NO);    \
        ERROR_LOG_STR("\n@ "); \
        ERROR_LOG_STR(__FILE__); \
        bsp_indication(0xff); \
    } \
} while(0);


/**
 * WDT function
 *
 */
void bsp_wdt_clear(void);

/**
 * delay
 *
 */
#define DELAY_2MS_POLLING_CNT (60000)


#define DELAY_2MS() \
do {\
    volatile uint16_t i;\
    for(i = 0; i < DELAY_2MS_POLLING_CNT; i++) {}\
} while(0) 

#define DELAY_10MS() \
do {\
    uint8_t i;\
    for(i = 0; i < 5; i++) {\
        DELAY_2MS();\
    }\
} while(0) 

/**
 * indication
 *  - used to indicate current application state
 *
 */
void bsp_indication(uint8_t indication);

#endif // _BSP_H_
