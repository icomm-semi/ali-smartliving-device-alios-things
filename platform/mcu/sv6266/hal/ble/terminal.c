/** 
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include "terminal.h"
#include "uart/drv_uart.h"
#include "uart/hal_uart.h"


void terminal_init() {

    
}

char putchar (char c) {
	uint8_t temp;
    if (c == '\n') {
    	temp = 0x0d;
        drv_uart_write_fifo(&temp, 1, UART_BLOCKING);
        while(!drv_uart_is_transmitter_idle());
        
    }
    drv_uart_write_fifo((uint8_t const *)&c, 1, UART_BLOCKING);
    while(!drv_uart_is_transmitter_idle());
    return 0;
    
}


void terminal_str(const char* str) {

    while(*str != '\0') {
        putchar(*str);
        str++;
    }
    
}

char itoa(uint8_t hex) {

    // limit
    hex &= 0x0f;

    if(hex >= 10) {
        hex += 'a' -10;
    }
    else {
        hex += '0';
    }

    return (char)hex;
}

void terminal_hex(uint8_t octet) {

    char x = octet;

    terminal_str(" 0x");

    x = x >> 4;
    putchar(itoa(x));
    putchar(itoa(octet));
}

void terminal_array(uint8_t* array, uint8_t array_size) {

    uint8_t idx;
    uint8_t raw = 0;

    terminal_str("\n           0    1    2    3    4    5    6    7\n 0x00: ");
    for(idx = 0, raw = 0; idx < array_size; idx++, raw++) {
        if(raw == 8) {
            putchar('\n');
            terminal_hex(idx);
            terminal_str(": ");
            raw = 0;
        }
        terminal_hex(*(array++));
    }
}
