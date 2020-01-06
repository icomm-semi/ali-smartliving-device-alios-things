/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
/**
 * wakelock implementation
 *
 */
#include "types.h"
#include "terminal.h"

#include "icmd_interface.h"
#include "icmd_interface_hal.h"
 
int8_t g_wakelock_nest_cnt = 0;

int8_t icmd_set_wakelock(bool wakelock) {

    bool    wakelock_cs;

    wakelock_cs = false;
    if(g_wakelock_nest_cnt > 0) {
        wakelock_cs = true;
    }
    // TERMINAL_STR("\n! WAKELOCK CS: "); TERMINAL_HEX(wakelock_cs);

    if(wakelock) {
        g_wakelock_nest_cnt++;
    }
    else {
        g_wakelock_nest_cnt--;
    }
    // TERMINAL_STR("\n! WAKELOCK NEST CNT: "); TERMINAL_HEX(g_wakelock_nest_cnt);

    // limit
    if(g_wakelock_nest_cnt < 0) {
        g_wakelock_nest_cnt = 0;
    }

    if(wakelock_cs == false) {
        if(g_wakelock_nest_cnt > 0) {
            if(icmd_interface_wakelock2status(true)) {
                return ICMD_INTERFACE_FAIL;
            }
            return 0;
        }
    }
    else {
        if(g_wakelock_nest_cnt == 0) {
            if(icmd_interface_wakelock2status(false)) {
                return ICMD_INTERFACE_FAIL;
            }
            return 0;
        }
    }
    return 0;
}

void icmd_set_wakelock_init(void) {
    
    icmd_interface_wakelock(true);
    g_wakelock_nest_cnt = 1;
}
