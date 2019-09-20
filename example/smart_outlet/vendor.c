/*
 *copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include "netmgr.h"
#include "iot_export.h"
#include "iot_import.h"
#include "app_entry.h"
#include "aos/kv.h"
#include <hal/soc/gpio.h>
#include "vendor.h"

#define LED_GPIO    8
#define RELAY_GPIO  24
#ifdef ON_EVB
#define KEY_GPIO    25
#else
#define KEY_GPIO    12
#endif

static gpio_dev_t io_led;
static gpio_dev_t io_key;
static gpio_dev_t io_relay;

void product_init(void)
{
    io_led.port = LED_GPIO;
    io_relay.port = RELAY_GPIO;
    io_key.port = KEY_GPIO;

    io_led.config = OUTPUT_PUSH_PULL;
    io_relay.config = OUTPUT_PUSH_PULL;
    io_key.config = INPUT_PULL_UP;

    hal_gpio_init(&io_led);
    hal_gpio_init(&io_relay);
    hal_gpio_init(&io_key);

    hal_gpio_output_low(&io_relay);
    hal_gpio_output_low(&io_led);
}

static int switch_state;
void product_set_switch(bool state)
{
    if (state) {
        hal_gpio_output_low(&io_relay);
    } else {
        hal_gpio_output_high(&io_relay);
    }
    switch_state = state;
}

bool product_get_switch(void)
{
    return switch_state;
}

static bool product_get_key(void)
{
    return hal_gpio_input_get(&io_key, NULL);
}

static int led_state;
static void product_set_led(bool state)
{
    if (state) {
        hal_gpio_output_low(&io_led);
    } else {
        hal_gpio_output_high(&io_led);
    }
    led_state = state;
}

static bool product_get_led()
{
    return led_state;
}

static void product_toggle_led()
{
    if (product_get_led() == ON) {
        product_set_led(OFF);
    } else {
        product_set_led(ON);
    }

}

int device_net_state = UNKNOW_STATE;
int get_net_state(void)
{
    return device_net_state;
}

void set_net_state(int state)
{
    device_net_state = state;
}

extern void do_awss_reset();
void key_detect_event_task(void)
{
    int press_key_cnt = 0;

    while (1) {
        if (!product_get_key()) {
            press_key_cnt++;
            LOG("press_key_cnt :%d\n", press_key_cnt);
        } else {
            if ((press_key_cnt >= 2) && (press_key_cnt < 100)) {
                if (product_get_switch() == ON) {
                    product_set_switch(OFF);
                    user_post_powerstate(OFF);
                } else {
                    product_set_switch(ON);
                    user_post_powerstate(ON);
                }
                press_key_cnt = 0;
            }
        }
        if (press_key_cnt >= 100) {
            do_awss_reset();
            break;
        }
        aos_msleep(100);
    }
}

static void indicate_net_state_task(void)
{
    uint32_t nCount = 0;
    uint32_t duration = 0;

    while (1) {
        if (UNCONFIGED == get_net_state()) {
            product_toggle_led();
        } else if (GOT_SSID == get_net_state() || CONNECT_FAILED == get_net_state()) {
            nCount++;
            if (nCount >= 3) {
                product_toggle_led();
                nCount = 0;
            }
        } else if (CONNECT_SUCCESS == get_net_state()) {
            product_set_led(ON);
        } else if (BIND_SUCCESS == get_net_state()) {
            nCount++;
            if (nCount >= 3 && duration < 10) {
                product_toggle_led();
                nCount = 0;
                duration ++;
            }
            if (duration >= 10) {
                duration = 0;
                set_net_state(CONNECT_SUCCESS);
            }
        }
        aos_msleep(200);
    }

    LOG("exit quick_light mode\n");
    aos_task_exit(0);
}

void check_net_config(void)
{
    int ret = 0;
    int32_t len = 0;

    netmgr_ap_config_t config;
    memset(&config, 0, sizeof(netmgr_ap_config_t));

    ret = netmgr_get_ap_config(&config);
    len = strlen(config.ssid);

    if (len <= 0) {
        LOG("no have wifi ssid can active_awss\n");
        set_net_state(UNCONFIGED);
        awss_config_press();
    } else {
        set_net_state(GOT_SSID);
    }
    aos_task_new("indicate net state", indicate_net_state_task, NULL, 4096);
}

int vendor_get_product_key(char *product_key, int *len)
{
    char *pk = NULL;
    int ret = -1;

    if ((pk = hfilop_layer_get_product_key()) != NULL) {
        *len = strlen(pk);
        memcpy(product_key, pk, *len);
        ret = 0;
    }

    return ret;
}

int vendor_get_product_secret(char *product_secret, int *len)
{
    char *ps = NULL;
    int ret = -1;

    if ((ps = hfilop_layer_get_product_secret()) != NULL) {
        *len = strlen(ps);
        memcpy(product_secret, ps, *len);
        ret = 0;
    }

    return ret;
}

int vendor_get_device_name(char *device_name, int *len)
{
    char *dn = NULL;
    int ret = -1;

    if ((dn = hfilop_layer_get_device_name()) != NULL) {
        *len = strlen(dn);
        memcpy(device_name, dn, *len);
        ret = 0;
    }

    return ret;
}

int vendor_get_device_secret(char *device_secret, int *len)
{
    char *ds = NULL;
    int ret = -1;
    if ((ds = hfilop_layer_get_device_secret()) != NULL) {
        *len = strlen(ds);
        memcpy(device_secret, ds, *len);
        ret = 0;
    }

    return ret;
}

void vendor_device_bind(void)
{
    set_net_state(BIND_SUCCESS);
}

void vendor_device_unbind(void)
{
    do_awss_reset();
}

