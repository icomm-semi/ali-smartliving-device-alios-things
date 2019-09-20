/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef __VENDOR_H__
#define __VENDOR_H__

#include <aos/aos.h>
typedef enum {
    UNCONFIGED = 0,
    GOT_SSID,
    CONNECT_SUCCESS,
    CONNECT_FAILED,
    BIND_SUCCESS,
    UNKNOW_STATE
} eNetState;

typedef enum {
    OFF = 0,
    ON
} eSwitchState;

void product_init(void);
void product_set_switch(bool state);
bool product_get_switch(void);

int get_net_state(void);
void set_net_state(int state);

void check_net_config(void);
void key_detect_event_task(void);

int vendor_get_product_key(char *product_key, int *len);
int vendor_get_product_secret(char *product_secret, int *len);
int vendor_get_device_name(char *device_name, int *len);
int vendor_get_device_secret(char *device_secret, int *len);

void vendor_device_bind(void);
void vendor_device_unbind(void);

#endif
