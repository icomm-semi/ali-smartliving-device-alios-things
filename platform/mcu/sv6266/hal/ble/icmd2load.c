/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#include <stdarg.h>
#include <string.h>

#include "types.h"
#include "terminal.h"
#include "ble_bsp.h"

#include "icmd_api.h"
#include "icmd.h"
#include "icmd2load.h"
#include "icmd_interface.h"
#include "icmd_interface_hal.h"

/**
 * status storage
 *
 */
	XDATA status_t g_status;
	
	int8_t icmd2load(const uint8_t* icmd, uint16_t icmd_length_max) {
	
		uint8_t op;
		uint8_t param_len;
	
		uint8_t len;
		uint8_t loop_i;
		uint8_t write_buf[70];
		// # check/dispatch header
		if(ICMD_HEADER_LEN > icmd_length_max) {
			TERMINAL_STR("\n! icmd header overflow");
			return -1;
		}
		len = ICMD_HEADER_LEN;
		// ## information from header
		op			= icmd[ICMD_OP_INDEX];
		param_len	= icmd[ICMD_PARAM_LEN_INDEX];
	
		// # check param
		len += param_len;
		if(len > icmd_length_max) {
			TERMINAL_STR("! param overflow\n");
			return -1;
		}
	
		if(icmd_set_wakelock(true)) {
			ERROR_LOG_STR("\n ! WAKELOCK exception");
			goto exception;
		}
	
		// # write
		icmd_interface_write_in_progress(true);
	
		for(loop_i = 0; loop_i < len; loop_i++) {
			write_buf[loop_i]=icmd[loop_i];
			
		};
		icmd_interface_write_buf(write_buf,len);
		icmd_interface_write_in_progress(false);
	
		// # cmd without status
		g_status.param[STATUS_PARAM_STATUS_CODE_INDEX] = 0x00;
		if(op == ICMD_OP_RESET) {
			goto icmd_done;
		}
		if(op == ICMD_OP_RESERVED51) {
			goto icmd_done;
		}
		
		g_status.param[STATUS_PARAM_STATUS_CODE_INDEX] = 0xff;
		if(icmd_interface_query_status((void*)&g_status, sizeof(g_status))) {
			goto exception;
		}
#if BLE_DEBUG_SPI_DATA		
		TERMINAL_STR("\n# g_status1: "); TERMINAL_ARRAY((void*)&g_status,sizeof( g_status ));
#endif
		status_handler();	
	icmd_done:
		// # wakelock disable/
		
		if(icmd_set_wakelock(false)) {
			ERROR_LOG_STR("\n ! WAKELOCK exception");
			goto exception;
		}
		
		// clear WDT, after successful cmd.
		bsp_wdt_clear();
	
		return g_status.param[STATUS_PARAM_STATUS_CODE_INDEX]; 
	
	exception:
		ERROR_LOG_STR("\n! icmd2load exception");
		return -1;
	}
	
	int16_t icmd_tbl2load(const uint8_t* icmd, uint16_t icmd_tbl_size) {
	
		uint16_t		loop_i;
		const uint8_t*	loop_icmd;
		int8_t			loop_icmd_length;
	
		loop_icmd	= icmd;
		loop_i		= 0;
		
		if(icmd_set_wakelock(true)) {
			ERROR_LOG_STR("\n ! WAKELOCK exception");
			return -1;
		}
	
		while(loop_i < icmd_tbl_size) {
			if(icmd2load(loop_icmd, icmd_tbl_size -loop_i)) {
				return -1;
			}
			loop_icmd_length = ICMD_HEADER_LEN +loop_icmd[STATUS_PARAM_LEN_INDEX];
			//
			loop_i		+= loop_icmd_length;
			loop_icmd	+= loop_icmd_length;

		}
		
		if(icmd_set_wakelock(false)) {
			ERROR_LOG_STR("\n ! WAKELOCK exception");
			return -1;
		}
		
		return loop_i;
	}
	
	/**
	 * when hardfault happen, please adopt reset
	 *
	 */
	DECLARE_CONST uint8_t reset[] = {ICMD_INDICATOR, ICMD_OP_RESET, 0};
	void icmd_reset() {
	
		uint8_t loop_i;
		
		icmd_interface_write_in_progress(true);
		for(loop_i = 0; loop_i < sizeof(reset); loop_i++) {
			icmd_interface_write(reset[loop_i]);
		};
		icmd_interface_write_in_progress(false);
		
		// implement 2ms delay
		DELAY_2MS();
	}

