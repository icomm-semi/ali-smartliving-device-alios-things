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

#include "types.h"
#include "icmd_api.h"
#include "icmd.h"
#include "icmd_interface.h"
#include "icmd_interface_hal.h"
#include "icmd2load.h"
#include "ble_bsp.h"

#include "terminal.h"



/**
 * ICMD interface operation
 *
 */
ICMD_INTERFACE_STATUS icmd_interface_write_in_progress(bool in_progress) {
    
#ifdef INTERFACE_CMD_LOG
    if(g_icmd_interface_cmd_log_enable == true) {
        if(in_progress) {
            TERMINAL_STR("\n/* CMD */");
        }
    }
#endif
    return ICMD_INTERFACE_SUCCESS;
}

ICMD_INTERFACE_STATUS icmd_interface_write(uint8_t raw) {

#ifdef INTERFACE_CMD_LOG
    if(g_icmd_interface_cmd_log_enable == true) {
        TERMINAL_HEX(raw);
        TERMINAL_STR(",");
    }
#endif
    icmd_interface_spim_tx(raw);

    return ICMD_INTERFACE_SUCCESS;
}
ICMD_INTERFACE_STATUS icmd_interface_write_buf(uint8_t* raw, uint8_t len) {

#ifdef INTERFACE_CMD_LOG
    if(g_icmd_interface_cmd_log_enable == true) {
        TERMINAL_HEX(raw);
        TERMINAL_STR(",");
    }
#endif
	icmd_interface_spim_tx_buf(raw, len);
    return ICMD_INTERFACE_SUCCESS;
}

#ifdef INTERFACE_CMD_LOG
bool g_icmd_interface_cmd_log_enable = false;
#endif

/**
 * STATUS and IEVT
 *
 */
#ifndef IEVT_INTERFACE_BUF_DEPTH
#define IEVT_INTERFACE_BUF_DEPTH (4)
#endif

#define IEVT_INTERFACE_BUF_SIZE ((IEVT_INTERFACE_BUF_DEPTH) * sizeof(ievt_t) +sizeof(status_t))
uint8_t XDATA   sg_ievt_interface_buf[IEVT_INTERFACE_BUF_SIZE];
int             sg_ievt_interface_buf_wr_idx = 0;


ICMD_INTERFACE_STATUS ievt_interface_buf_load(uint8_t* raw, uint8_t len_max) {

    uint8_t len;
    int rd_idx =0;

    if(sg_ievt_interface_buf_wr_idx == 0) {
        return ICMD_INTERFACE_FAIL;
    }

    len = IEVT_HEADER_LEN +sg_ievt_interface_buf[IEVT_PARAM_LEN_INDEX];
    if(len > len_max) {
        return ICMD_INTERFACE_FAIL;
    }
    memcpy(raw, sg_ievt_interface_buf, len);
    rd_idx += len;

    sg_ievt_interface_buf_wr_idx -= rd_idx;
    if(sg_ievt_interface_buf_wr_idx) {
        memcpy(sg_ievt_interface_buf, sg_ievt_interface_buf +rd_idx, sg_ievt_interface_buf_wr_idx);
    }
    return ICMD_INTERFACE_SUCCESS;
}

ICMD_INTERFACE_STATUS icmd_spi_query(uint8_t* raw, uint16_t raw_length_max) {

    uint8_t index;
    uint8_t byte2read;
    uint8_t intermediate;
    uint8_t sts,i;
	uint8_t status_length;
	uint8_t read_query_buf[30];
    // # do query
    icmd_interface_poll_spim_trx(POLL_INDICATOR, &sts, &status_length);
    //TERMINAL_STR("\n# sts: "); TERMINAL_HEX(sts);
    //TERMINAL_STR("\n# status_length: "); TERMINAL_HEX(status_length);

    if((sts & 0x04) == 0x04) {
        ERROR_LOG_STR("\n! [ievt query]SPI STS TRX error: "); ERROR_LOG_HEX(sts);
        goto exception;
    }

    if((sts & 0x01) == 0x00) {
        ERROR_LOG_STR("\n! SPI STS nothing to query");  ERROR_LOG_HEX(sts);
        goto exception;
    }

    if(raw_length_max < STATUS_HEADER_LEN) {
        goto exception;
    }
	icmd_interface_query_spim_trx(QUERY_INDICATOR, read_query_buf, status_length);
	
	for(i=0; i < status_length; i++)
	{
		*raw = read_query_buf[i+1];
		raw++;
		
	}
    // turn on irq, if level trigger
    return ICMD_INTERFACE_SUCCESS;

exception:
    return ICMD_INTERFACE_FAIL;
}

#define DELAY_10US() \
do{\
    volatile int i = 0;\
    for(i = 0; i < 10; i++) {}\
}while(0)

ICMD_INTERFACE_STATUS icmd_interface_query_status(uint8_t* raw, uint8_t raw_length_max) {

    uint8_t* buf;

    uint16_t length;
    uint16_t length_max;
#ifdef EARLY_HANDLE_IEVT
    uint8_t is_handled;
#endif

wait_status:
    DELAY_10US();
    while(icmd_interface_irq() != IEVT_IRQ_ACTIVE) {};
    // # do query
    // ## buffer
    buf         = sg_ievt_interface_buf + sg_ievt_interface_buf_wr_idx;
    length_max  = sizeof(sg_ievt_interface_buf) -sg_ievt_interface_buf_wr_idx;
    if(length_max < sizeof(status_t)) {
        return ICMD_INTERFACE_FAIL; // NO enough space to hold a status.
    }
    if(icmd_spi_query(buf, length_max) != ICMD_INTERFACE_SUCCESS) {
        return ICMD_INTERFACE_FAIL;
    }
    
    length = STATUS_HEADER_LEN +buf[STATUS_PARAM_LEN_INDEX];

    // Got iEVT:
    //  check if iEVT fully buffered.
    //  update buffer index
    //  increment ievt counter
    if(buf[IEVT_INDICATOR_INDEX] == IEVT_INDICATOR) {
        if(length > length_max) {
            return ICMD_INTERFACE_FAIL;
        }

#ifdef EARLY_HANDLE_IEVT
        icmd_interface_early_handle_ievt(&is_handled, (void *)buf);
        if (is_handled == 0x00)
        {
            sg_ievt_interface_buf_wr_idx += length;
        }
        else
        {
            ievt_count_decrement();
        }
#else

        sg_ievt_interface_buf_wr_idx += length;

#endif

        goto wait_status;
    }
    // STATUS
    if(length > length_max) {
        length = length_max;
    }
    if(length > raw_length_max) {
        length = raw_length_max;
    }
    memcpy(raw, buf, length);

    // in SPI, STATUS, as well as iEVT, would trigger IRQ.
    ievt_count_decrement();
    
    return ICMD_INTERFACE_SUCCESS;
}

ICMD_INTERFACE_STATUS icmd_interface_read_ievt(uint8_t* raw, uint8_t raw_length_max) {

    // check if ievt buffer not empty
    if(sg_ievt_interface_buf_wr_idx) {
        return ievt_interface_buf_load(raw, raw_length_max);
    }
   
    // check spi-irq, if not active, break

    if(icmd_interface_irq() != IEVT_IRQ_ACTIVE) {
        return ICMD_INTERFACE_SUCCESS;
    }

    // spi query directly
    if(icmd_spi_query(raw, raw_length_max) != ICMD_INTERFACE_SUCCESS) {
		TERMINAL_STR("\n# ICMD_INTERFACE_FAIL");
        return ICMD_INTERFACE_FAIL;
    }
	
    return ICMD_INTERFACE_SUCCESS;
}


/**
 * WAKELOCK (P2.1)
 *  set WAKELOCK pin level of ICMD
 *
 */
ICMD_INTERFACE_STATUS icmd_interface_wakelock2status(bool wakelock) {
    
    ICMD_INTERFACE_STATUS status;
    uint8_t status_buf[STATUS_HEADER_LEN + STATUS_PARAM_ICMD_OP_SIZE];
    
    icmd_interface_wakelock(wakelock);
    status = ICMD_INTERFACE_SUCCESS;
    if(icmd_interface_query_status(status_buf, sizeof(status_buf)) != ICMD_INTERFACE_SUCCESS){
        status = ICMD_INTERFACE_FAIL;
    }
    if(status_buf[STATUS_PARAM_INDEX] != ICMD_OP_WAKELOCK) {
        status = ICMD_INTERFACE_FAIL;
    }        
    return status;
}
