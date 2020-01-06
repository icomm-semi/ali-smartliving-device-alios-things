/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#include "icmd.h"
#include "icmd_interface_hal.h"
#include "icmd_interface.h"
#include "icmd_api.h"
#include "ievt_handler.h"
#include "ble_bsp.h"

void ievt_irq_handler(void) {

    g_ievt_count++;
}

