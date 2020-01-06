/** 
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/**
 * implementation of get_ievt()
 *
 */
#include <string.h>

#include "types.h"
#include "icmd_api.h"
#include "icmd.h"
#include "icmd_interface.h"
#include "terminal.h"

#include "ble_bsp.h"

int8_t g_ievt_count = 0; 
void ievt_count_decrement(void){
    
    if(g_ievt_count == 0) {
        return;
    }
    
    bsp_irq_disable();  // critical section begin
    g_ievt_count--;
    bsp_irq_enable();   // critical section end
}

// # nop_ievt
DECLARE_CONST uint8_t nop_ievt[] = {IEVT_INDICATOR, IEVT_OP_NOP, 0x00};

int8_t get_ievt(ievt_t* ievt) {

    int8_t status = 0;

    // clear ievt stuffs
    memcpy(ievt, nop_ievt, sizeof(nop_ievt));
    if(icmd_interface_read_ievt((void*)ievt, sizeof(ievt_t)) != ICMD_INTERFACE_SUCCESS) {
        status = -1;
        TERMINAL_STR("\n# status = -1; ");
    }
    ievt_count_decrement();
	//TERMINAL_STR("\n# ievt: "); TERMINAL_ARRAY((void*)ievt,sizeof( ievt_t ));
    // action
    return status;
}
