/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef _ICMD_INTERFACE_
#define _ICMD_INTERFACE_

/**
 * iCMD interface prototypes, implemented on different physical interface.
 *  - this functions would be invoked by iCMD APIs.
 *
 */
#include "types.h"

/**
 * ICMD interface initialization 
 *
 */
typedef enum {
    ICMD_INTERFACE_FAIL     = -1,
    ICMD_INTERFACE_SUCCESS  = 0
}   ICMD_INTERFACE_STATUS;

/**
 * ICMD interface operation
 *
 */
ICMD_INTERFACE_STATUS icmd_interface_write_in_progress(bool in_progress);
ICMD_INTERFACE_STATUS icmd_interface_write            (uint8_t raw);
ICMD_INTERFACE_STATUS icmd_interface_write_buf(uint8_t* raw,uint8_t len);

#ifdef INTERFACE_CMD_LOG
extern bool g_icmd_interface_cmd_log_enable;
#endif

/**
 * STATUS and IEVT
 *
 */
ICMD_INTERFACE_STATUS icmd_interface_query_status(uint8_t* raw, uint8_t raw_length_max);
ICMD_INTERFACE_STATUS icmd_interface_read_ievt   (uint8_t* raw, uint8_t raw_length_max);
/**
 * WAKELOCK
 *  set WAKELOCK pin level of ICMD
 *
 */
ICMD_INTERFACE_STATUS icmd_interface_wakelock2status(bool wakelock);

/**
 * BLE_IRQ
 *
 */





#if defined (UART_INTERFACE)
/**
 * UART interface specific
 *  - about baudrate
 *
 */
#define ICMD_UART_INTERFACE_BAUDRATE_9600       9600
#define ICMD_UART_INTERFACE_BAUDRATE_19200      19200
#define ICMD_UART_INTERFACE_BAUDRATE_38400      38400
#define ICMD_UART_INTERFACE_BAUDRATE_57600      57600

#define ICMD_UART_INTERFACE_BAUDRATE            ICMD_UART_INTERFACE_BAUDRATE_9600

#ifndef UART_BAUDRATE_ON_PATCH
#define UART_BAUDRATE_ON_PATCH                  ICMD_UART_INTERFACE_BAUDRATE 
#endif

#if ((UART_BAUDRATE_ON_PATCH != ICMD_UART_INTERFACE_BAUDRATE_9600) &&   \
     (UART_BAUDRATE_ON_PATCH != ICMD_UART_INTERFACE_BAUDRATE_19200) &&  \
     (UART_BAUDRATE_ON_PATCH != ICMD_UART_INTERFACE_BAUDRATE_38400) &&  \
     (UART_BAUDRATE_ON_PATCH != ICMD_UART_INTERFACE_BAUDRATE_57600))
#error "Please set your UART_BAUDRATE_ON_PATCH as 9600, 19200, 38400 or 57600."
#endif

#if (ICMD_UART_INTERFACE_BAUDRATE != UART_BAUDRATE_ON_PATCH)
bool icmd_interface_check_uart_baudrate_updated(void);
void icmd_interface_update_uart_baudrate(void);
void icmd_interface_reset_uart_baudrate(void);
#endif // ICMD_UART_INTERFACE_BAUDRATE != UART_BAUDRATE_ON_PATCH
#endif // (UART_INTERFACE)

#endif // _ICMD_INTERFACE_
