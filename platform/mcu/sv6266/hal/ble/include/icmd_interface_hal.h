/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef _ICMD_INTERFACE_HAL_
#define _ICMD_INTERFACE_HAL_

/**
 * iCMD interface hardware abstration obstraction layer prototypes, SHALL be implemented on different physical interface.
 *
 */
#include "types.h"
#include "icmd_interface.h"



/**
 *  interface
 *
 */

void icmd_interface_spim_tx_buf(uint8_t* raw, uint8_t len);




/**
 * SPI interface
 *
 */
void icmd_interface_spim_tx (uint8_t raw);
void icmd_interface_spim_trx(uint8_t tx, uint8_t* rx);
void icmd_interface_poll_spim_trx(uint8_t tx, uint8_t* rx, uint8_t* rx_length);
void icmd_interface_query_spim_trx(uint8_t tx, uint8_t* rx, uint8_t tx_length);

/* SPI Burst Interface */
void icmd_interface_spim_burst_tx(const uint8_t* raw, uint8_t raw_length);
void icmd_interface_spim_burst_rx(uint8_t* raw, uint8_t raw_length_max, uint8_t spi_length);

/**
 * WAKELOCK
 *  set WAKELOCK pin level of ICMD
 *
 */
void icmd_interface_wakelock(bool wakelock);

/**
 * IRQ
 *  check IRQ level of ICMD
 *
 */
bool icmd_interface_irq(void);

/**
 * RSTN
 *  - control HW_RSTN
 *  - only necessary if MCU and Condor are powered seperately.
 *
 */
void icmd_interface_rstn(void);

#endif // _ICMD_INTERFACE_HAL_
