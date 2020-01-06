/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef _TERMINAL_H_
#define _TERMINAL_H_

/**
 * terminal prototypes, SHALL be implemented on different platform.
 *
 */
#include "types.h"


/**
 * initialization
 *
 */
void terminal_init(void);

void terminal_str   (const char* str);
void terminal_hex   (uint8_t octet);
void terminal_array (uint8_t* array, uint8_t array_size);

int printf(const char* format, ...);

/**
 * terminal log
 *
 */
#define TERMINAL_ON
#ifdef TERMINAL_ON
#define TERMINAL_STR(_STR)      terminal_str(_STR)
#define TERMINAL_HEX(_HEX)      terminal_hex(_HEX)
#define TERMINAL_ARRAY(_ARRAY, _ARRAY_SIZE)  terminal_array(_ARRAY,  _ARRAY_SIZE)
#else
#define TERMINAL_STR(_STR)
#define TERMINAL_HEX(_HEX)
#define TERMINAL_ARRAY(_ARRAY, _ARRAY_SIZE)
#endif
#define ERROR_LOG_ON
#ifdef ERROR_LOG_ON
#define ERROR_LOG_STR(_STR)      terminal_str(_STR)
#define ERROR_LOG_HEX(_HEX)      terminal_hex(_HEX)
#define ERROR_LOG_ARRAY(_ARRAY, _ARRAY_SIZE) terminal_array(_ARRAY,  _ARRAY_SIZE)
#else
#define ERROR_LOG_STR(_STR)
#define ERROR_LOG_HEX(_HEX)
#define ERROR_LOG_ARRAY(_ARRAY, _ARRAY_SIZE)
#endif

#define TERMINAL_HEX16(_HEX) \
do { \
    TERMINAL_HEX((_HEX) >> 8); \
    TERMINAL_HEX((_HEX)); \
} while(0)

#define TERMINAL_HEX32(_HEX) \
do { \
    TERMINAL_HEX((_HEX) >> 24); \
    TERMINAL_HEX((_HEX) >> 16); \
    TERMINAL_HEX((_HEX) >> 8); \
    TERMINAL_HEX((_HEX)); \
} while(0)

#endif // _TERMINAL_H_
