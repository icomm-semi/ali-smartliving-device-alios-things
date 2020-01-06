/** 
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef _TYPES_H_
#define _TYPES_H_

/**
 * MCU specific type defintion
 *
 */
#if 1
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
//typedef unsigned      		int uint32_t;

typedef unsigned           int uint32;
typedef   signed          char int8_t;
typedef   signed short     int int16_t;
//typedef   signed           int int32_t;
typedef   unsigned          char bool;




#endif
#define	PACKED	



#define SUCCESS     0

#ifndef true
#define true        1
#endif

#ifndef false
#define false       0
#endif

#ifndef NULL
#define NULL        (void *)0
#endif

/**
 * type conversion
 *
 */
#define U8_TO_U16(_SRC_) \
    ((((uint16_t)((_SRC_)[1])) << 8) | ((uint16_t)((_SRC_)[0])))
#define U16_TO_U8(_SRC_, _DST_) \
    do { \
        (_DST_)[1] = (uint8_t)((_SRC_) >> 8); \
        (_DST_)[0] = (uint8_t)(_SRC_); \
    } while(0)
#define U8(_SRC_)  ((uint8_t)(_SRC_))
#define U16(_SRC_) ((uint16_t)(_SRC_))
#define PTR(_SRC_) ((uint8_t*)(_SRC_))
#define VPTR(_SRC_) ((uint8_t*)(_SRC_))

#define _MACRO2STRING(str) #str
#define MACRO2STRING(str) _MACRO2STRING(str)

/**
 * toolchain specific types
 *
 */
#ifdef __C51__
#define DECLARE_CONST   code
#define XDATA   xdata
#else
#define DECLARE_CONST   const
#define XDATA
#endif
///TODO: add packed qualifier for different mcu


#endif  // _TYPES_H_
