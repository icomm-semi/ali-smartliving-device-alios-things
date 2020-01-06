/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef _ICMD2LOAD_H_
#define _ICMD2LOAD_H_

#include "types.h"
#include "icmd.h"

int8_t      icmd2load       (const uint8_t* icmd, uint16_t icmd_length_max);
int8_t      icmd2load_2     (uint8_t* icmd, uint16_t icmd_length_max);
int16_t     icmd_tbl2load   (const uint8_t* icmd, uint16_t icmd_tbl_size);

/**
 * status structure
 *  - at least 2 parameter bytes required, for corresponding CMD OP and status code. 
 *
 */
#ifndef STATUS_PARAM_LEN_MAX
#define STATUS_PARAM_LEN_MAX 8
#endif

typedef struct {
    // header
    uint8_t indicator;
    uint8_t op;
    uint8_t param_len;
    // param
    uint8_t param[STATUS_PARAM_LEN_MAX];

} PACKED status_t;

// STATUS storage
extern XDATA status_t g_status;

/**
 * STATUS handler, please implement this handler in application.
 *  - in this handler, please check STATUS CODE and ICMD OP CODE.
 *
 */
void status_handler(void);
// special cmd
void icmd_reset(void);

#define LOAD_TBL(TBL) \
do { \
    if(icmd_tbl2load(&(TBL)[0], sizeof((TBL))) != sizeof((TBL))) { \
    }\
}while(0)
#endif // _ICMD2LOAD_H_
