/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifdef DEPRECATED_LINKKIT
#include "deprecated/gateway.c"
#else
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "cJSON.h"
#include "iot_import.h"
#include "iot_export_linkkit.h"
#include "aos/kv.h"

#if defined(OTA_ENABLED) && defined(BUILD_AOS)
    #include "ota_service.h"
#endif

// for demo only
#define PRODUCT_KEY      "a1RIsMLz2BJ"
#define PRODUCT_SECRET   "fSAF0hle6xL0oRWd"
#define DEVICE_NAME      "example1"
#define DEVICE_SECRET    "RDXf67itLqZCwdMCRrw0N5FHbv5D7jrE"

#define USER_EXAMPLE_YIELD_TIMEOUT_MS (200)

#define EXAMPLE_TRACE(...) \
    do { \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__); \
        HAL_Printf(__VA_ARGS__); \
        HAL_Printf("\033[0m\r\n"); \
    } while (0)

#define EXAMPLE_SUBDEV_ADD_NUM          2
#define EXAMPLE_SUBDEV_MAX_NUM          20
const iotx_linkkit_dev_meta_info_t subdevArr[EXAMPLE_SUBDEV_MAX_NUM] = {
#ifdef REGION_SINGAPORE
    {
        "a2DUsQ3C7T8",
        "Q5OX8RLGBo9PCq3M",
        "fyci_subdev_sing",
        "iyEqGnk5lb8bz6oiLKgacTGWwcsLmB9M"
    },
    {
        "a2IWy8PbZxP",
        "p9t22CYSxQffujBd",
        "fyci_subdev_raw_sing",
        "c8Jc8SuymnPvUfVyMMWTEAKpufApDDxQ"
    },
#elif REGION_GERMANY
    {
        "a2DUsQ3C7T8",
        "Q5OX8RLGBo9PCq3M",
        "fyci_subdev_sing",
        "iyEqGnk5lb8bz6oiLKgacTGWwcsLmB9M"
    },
    {
        "a2IWy8PbZxP",
        "p9t22CYSxQffujBd",
        "fyci_subdev_raw_sing",
        "c8Jc8SuymnPvUfVyMMWTEAKpufApDDxQ"
    },
#else /* Mainland(Shanghai) for default */
    {
        "a185scQ2c22",
        "rwPp08IDoKpNi0n2",
        "fyci_subdev",
        "kVbpNJDPUEEJGyEnYmUUaOZocqSub7Wc"
    },
    {
        "a1y80y4lvBy",
        "XnNOHUi0FPWb9qFF",
        "fyci_subdev_raw",
        "YlqeyeIPjsWBbTAadyEnEuXFrJrNqHO0"
    },
#endif
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_03",
        "MjWMvCLBcuZyqUswryBbgypN8uOgJGVD"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_04",
        "DXbcbpxepIyYm3BiS0ICdBou4uWPfP6L"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_05",
        "VKuVZfcz3umcoR3WhOp4cu1p2dyTQGq1"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_06",
        "QTobiz1BdGW5XNgLGIgNSylH0btVvvGS"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_07",
        "IX7ol50rRS2uP8V74jt0DKfmYn8iC6h1"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_08",
        "sbFxx62evXVoVgJ5gL2oCLcz1pX9d6K2"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_09",
        "S0CpOl54GZxEO7Gz5DWQa5YxgUMfT4xA"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_10",
        "MowJJjiNIkTdUcX5fCNUDu39Yz02KADL"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_11",
        "BrJNdAihVznMWTpdRfe8HIiI95ubSYdN"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_12",
        "Yd3ZHK8D6cAKKRQb9rUevCfwPf7atoQ4"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_13",
        "d3HKvu2eBR5ytcgDaBEt0gpvJZlu9W0g"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_14",
        "qAJYUpQ1tGmAINQBzMiZwwbyjY6YXDGc"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_15",
        "GxgVknnAmUmwjjdHJf9dbEBDoqyDaUfp"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_16",
        "9d17Sv05j1XeTYOs80UBpBU1OYTTJ58X"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_17",
        "FTUm4HAfhZ5wH2u0pPn7PWcCLGDrgEfn"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_18",
        "mF7a2ptc3PRi7jWLE92t0GElhGdPnAe3"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_19",
        "Vrh8HuNvu3jtTEwSzulAjTqgOseAsVgz"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_20",
        "8Wxrxnjch6SW0s2HR5JkIBtgjt3BOUo7"
    }
};

typedef struct {
    int auto_add_subdev;
    int master_devid;
    int cloud_connected;
    int master_initialized;
    int subdev_index;
    int permit_join;
    void *g_user_dispatch_thread;
    int g_user_dispatch_thread_running;
} user_example_ctx_t;

static user_example_ctx_t g_user_example_ctx;

void *example_malloc(size_t size)
{
    return HAL_Malloc(size);
}

void example_free(void *ptr)
{
    HAL_Free(ptr);
}

static user_example_ctx_t *user_example_get_ctx(void)
{
    return &g_user_example_ctx;
}

static int user_connected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Cloud Connected");

    user_example_ctx->cloud_connected = 1;
    return 0;
}

static int user_disconnected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Cloud Disconnected");

    user_example_ctx->cloud_connected = 0;

    return 0;
}

static int user_down_raw_data_arrived_event_handler(const int devid, const unsigned char *payload,
        const int payload_len)
{
    EXAMPLE_TRACE("Down Raw Message, Devid: %d, Payload Length: %d", devid, payload_len);
    return 0;
}

static int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
        const char *request, const int request_len,
        char **response, int *response_len)
{
    EXAMPLE_TRACE("Service Request Received, Devid: %d, Service ID: %.*s, Payload: %s", devid, serviceid_len,
                  serviceid,
                  request);

    return 0;
}
#ifdef ALCS_ENABLED
static int user_property_get_event_handler(const int devid, const char *request, const int request_len, char **response,
        int *response_len)
{
    cJSON *request_root = NULL, *item_propertyid = NULL;
    cJSON *response_root = NULL;
    int index = 0;
    EXAMPLE_TRACE("Property Get Received, Devid: %d, Request: %s", devid, request);

    /* Parse Request */
    request_root = cJSON_Parse(request);
    if (request_root == NULL || !cJSON_IsArray(request_root)) {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }

    /* Prepare Response */
    response_root = cJSON_CreateObject();
    if (response_root == NULL) {
        EXAMPLE_TRACE("No Enough Memory");
        cJSON_Delete(request_root);
        return -1;
    }

    for (index = 0; index < cJSON_GetArraySize(request_root); index++) {
        item_propertyid = cJSON_GetArrayItem(request_root, index);
        if (item_propertyid == NULL || !cJSON_IsString(item_propertyid)) {
            EXAMPLE_TRACE("JSON Parse Error");
            cJSON_Delete(request_root);
            cJSON_Delete(response_root);
            return -1;
        }

        EXAMPLE_TRACE("Property ID, index: %d, Value: %s", index, item_propertyid->valuestring);

        if (strcmp("WIFI_Tx_Rate", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "WIFI_Tx_Rate", 1111);
        } else if (strcmp("WIFI_Rx_Rate", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "WIFI_Rx_Rate", 2222);
        } else if (strcmp("RGBColor", item_propertyid->valuestring) == 0) {
            cJSON *item_rgbcolor = cJSON_CreateObject();
            if (item_rgbcolor == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }
            cJSON_AddNumberToObject(item_rgbcolor, "Red", 100);
            cJSON_AddNumberToObject(item_rgbcolor, "Green", 100);
            cJSON_AddNumberToObject(item_rgbcolor, "Blue", 100);
            cJSON_AddItemToObject(response_root, "RGBColor", item_rgbcolor);
        } else if (strcmp("HSVColor", item_propertyid->valuestring) == 0) {
            cJSON *item_hsvcolor = cJSON_CreateObject();
            if (item_hsvcolor == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }
            cJSON_AddNumberToObject(item_hsvcolor, "Hue", 50);
            cJSON_AddNumberToObject(item_hsvcolor, "Saturation", 50);
            cJSON_AddNumberToObject(item_hsvcolor, "Value", 50);
            cJSON_AddItemToObject(response_root, "HSVColor", item_hsvcolor);
        } else if (strcmp("HSLColor", item_propertyid->valuestring) == 0) {
            cJSON *item_hslcolor = cJSON_CreateObject();
            if (item_hslcolor == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }
            cJSON_AddNumberToObject(item_hslcolor, "Hue", 70);
            cJSON_AddNumberToObject(item_hslcolor, "Saturation", 70);
            cJSON_AddNumberToObject(item_hslcolor, "Lightness", 70);
            cJSON_AddItemToObject(response_root, "HSLColor", item_hslcolor);
        } else if (strcmp("WorkMode", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "WorkMode", 4);
        } else if (strcmp("NightLightSwitch", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "NightLightSwitch", 1);
        } else if (strcmp("Brightness", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "Brightness", 30);
        } else if (strcmp("LightSwitch", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "LightSwitch", 1);
        } else if (strcmp("ColorTemperature", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "ColorTemperature", 2800);
        } else if (strcmp("PropertyCharacter", item_propertyid->valuestring) == 0) {
            cJSON_AddStringToObject(response_root, "PropertyCharacter", "testprop");
        } else if (strcmp("Propertypoint", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "Propertypoint", 50);
        } else if (strcmp("LocalTimer", item_propertyid->valuestring) == 0) {
            cJSON *array_localtimer = cJSON_CreateArray();
            if (array_localtimer == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }

            cJSON *item_localtimer = cJSON_CreateObject();
            if (item_localtimer == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                cJSON_Delete(array_localtimer);
                return -1;
            }
            cJSON_AddStringToObject(item_localtimer, "Timer", "10 11 * * * 1 2 3 4 5");
            cJSON_AddNumberToObject(item_localtimer, "Enable", 1);
            cJSON_AddNumberToObject(item_localtimer, "IsValid", 1);
            cJSON_AddItemToArray(array_localtimer, item_localtimer);
            cJSON_AddItemToObject(response_root, "LocalTimer", array_localtimer);
        }
    }
    cJSON_Delete(request_root);

    *response = cJSON_PrintUnformatted(response_root);
    if (*response == NULL) {
        EXAMPLE_TRACE("No Enough Memory");
        cJSON_Delete(response_root);
        return -1;
    }
    cJSON_Delete(response_root);
    *response_len = strlen(*response);

    EXAMPLE_TRACE("Property Get Response: %s", *response);

    return SUCCESS_RETURN;
}
#endif

static int user_property_cloud_error_handler(const int code, const char *data, const char *detail)
{
        EXAMPLE_TRACE("code =%d ,data=%s, detail=%s", code, data,detail);
}
static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    EXAMPLE_TRACE("Property Set Received, Devid: %d, Request: %s", devid, request);

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)request, request_len);
    EXAMPLE_TRACE("Post Property Message ID: %d", res);

    return 0;
}

static int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
        const int reply_len)
{
    const char *reply_value = (reply == NULL) ? ("NULL") : (reply);
    const int reply_value_len = (reply_len == 0) ? (strlen("NULL")) : (reply_len);

    EXAMPLE_TRACE("Message Post Reply Received, Devid: %d, Message ID: %d, Code: %d, Reply: %.*s", devid, msgid, code,
                  reply_value_len,
                  reply_value);
    return 0;
}

static int user_trigger_event_reply_event_handler(const int devid, const int msgid, const int code, const char *eventid,
        const int eventid_len, const char *message, const int message_len)
{
    EXAMPLE_TRACE("Trigger Event Reply Received, Devid: %d, Message ID: %d, Code: %d, EventID: %.*s, Message: %.*s", devid,
                  msgid, code,
                  eventid_len,
                  eventid, message_len, message);

    return 0;
}

static int user_timestamp_reply_event_handler(const char *timestamp)
{
    EXAMPLE_TRACE("Current Timestamp: %s", timestamp);

    return 0;
}

static int user_initialized(const int devid)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    EXAMPLE_TRACE("Device Initialized, Devid: %d", devid);

    if (user_example_ctx->master_devid == devid) {
        user_example_ctx->master_initialized = 1;
        user_example_ctx->subdev_index++;
    }

    return 0;
}

static uint64_t user_update_sec(void)
{
    static uint64_t time_start_ms = 0;

    if (time_start_ms == 0) {
        time_start_ms = HAL_UptimeMs();
    }

    return (HAL_UptimeMs() - time_start_ms) / 1000;
}

void user_post_property(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *property_payload = "{\"LightSwitch\":1}";

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)property_payload, strlen(property_payload));
    EXAMPLE_TRACE("Post Property Message ID: %d", res);
}
void user_post_sub_property(int id)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *property_payload = "{\"LightSwitch\":1}";

    res = IOT_Linkkit_Report(id, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)property_payload, strlen(property_payload));
    EXAMPLE_TRACE("Post Property Message ID: %d", res);
}

void user_post_event(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *event_id = "Error";
    char *event_payload = "{\"ErrorCode\":0}";

    res = IOT_Linkkit_TriggerEvent(user_example_ctx->master_devid, event_id, strlen(event_id),
                                   event_payload, strlen(event_payload));
    EXAMPLE_TRACE("Post Event Message ID: %d", res);
}

void user_deviceinfo_update(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *device_info_update = "[{\"attrKey\":\"abc\",\"attrValue\":\"hello,world\"}]";

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_DEVICEINFO_UPDATE,
                             (unsigned char *)device_info_update, strlen(device_info_update));
    EXAMPLE_TRACE("Device Info Update Message ID: %d", res);
}

void user_deviceinfo_delete(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *device_info_delete = "[{\"attrKey\":\"abc\"}]";

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_DEVICEINFO_DELETE,
                             (unsigned char *)device_info_delete, strlen(device_info_delete));
    EXAMPLE_TRACE("Device Info Delete Message ID: %d", res);
}

void user_post_raw_data(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    unsigned char raw_data[7] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_RAW_DATA,
                             raw_data, 7);
    EXAMPLE_TRACE("Post Raw Data Message ID: %d", res);
}

static int user_master_dev_available(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    if (user_example_ctx->cloud_connected && user_example_ctx->master_initialized) {
        return 1;
    }

    return 0;
}

void set_iotx_info()
{
    int len = 0;
    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char product_secret[PRODUCT_SECRET_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    char device_secret[DEVICE_SECRET_LEN + 1] = {0};

    memset(product_key, 0, sizeof(product_key));
    memset(product_secret, 0, sizeof(product_secret));
    memset(device_name, 0, sizeof(device_name));
    memset(device_secret, 0, sizeof(device_secret));

    len = PRODUCT_KEY_LEN+1;
    aos_kv_get("linkkit_product_key", product_key, &len);

    len = PRODUCT_SECRET_LEN+1;
    aos_kv_get("linkkit_product_secret", product_secret, &len);

    len = DEVICE_NAME_LEN+1;
    aos_kv_get("linkkit_device_name", device_name, &len);

    len = DEVICE_SECRET_LEN+1;
    aos_kv_get("linkkit_device_secret", device_secret, &len);    

    if((strlen(product_key) > 0) && (strlen(product_secret) > 0) \
       && (strlen(device_name) > 0) && (strlen(device_secret) > 0)) {
        HAL_SetProductKey(product_key);
        HAL_SetProductSecret(product_secret);
        HAL_SetDeviceName(device_name);
        HAL_SetDeviceSecret(device_secret);
        EXAMPLE_TRACE("pk[%s]", product_key);
        EXAMPLE_TRACE("ps[%s]", product_secret);
        EXAMPLE_TRACE("dn[%s]", device_name);
        EXAMPLE_TRACE("ds[%s]", device_secret);
    } else {
        HAL_SetProductKey(PRODUCT_KEY);
        HAL_SetProductSecret(PRODUCT_SECRET);
        HAL_SetDeviceName(DEVICE_NAME);
        HAL_SetDeviceSecret(DEVICE_SECRET);
        EXAMPLE_TRACE("pk[%s]", PRODUCT_KEY);
        EXAMPLE_TRACE("ps[%s]", PRODUCT_SECRET);
        EXAMPLE_TRACE("dn[%s]", DEVICE_NAME);
        EXAMPLE_TRACE("ds[%s]", DEVICE_SECRET);
    }
}

static int user_event_notify_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    EXAMPLE_TRACE("Event notify Received, Devid: %d, Request: %s", devid, request);

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_EVENT_NOTIFY_REPLY,
                             (unsigned char *)request, request_len);
    EXAMPLE_TRACE("Post Property Message ID: %d", res);

    return 0;
}

static int example_add_subdev(iotx_linkkit_dev_meta_info_t *meta_info)
{
    int res = 0, devid = -1;
    devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_SLAVE, meta_info);
    if (devid == FAIL_RETURN) {
        EXAMPLE_TRACE("subdev open Failed\n");
        return FAIL_RETURN;
    }
    EXAMPLE_TRACE("subdev open susseed, devid = %d\n", devid);

    res = IOT_Linkkit_Connect(devid);
    if (res == FAIL_RETURN) {
        EXAMPLE_TRACE("subdev connect Failed\n");
        return res;
    }
    EXAMPLE_TRACE("subdev connect success: devid = %d\n", devid);

    res = IOT_Linkkit_Report(devid, ITM_MSG_LOGIN, NULL, 0);
    if (res == FAIL_RETURN) {
        EXAMPLE_TRACE("subdev login Failed\n");
        return res;
    }
    #if defined(OTA_ENABLED) && defined(BUILD_AOS)
    static ota_service_t ctx = {0};
    memset(&ctx, 0, sizeof(ota_service_t));
    strncpy(ctx.pk, meta_info->product_key, sizeof(ctx.pk)-1);
    strncpy(ctx.dn, meta_info->device_name, sizeof(ctx.dn)-1);
    strncpy(ctx.ds, meta_info->product_secret, sizeof(ctx.ds)-1);
    ctx.trans_protcol = 0;
    ctx.dl_protcol = 3;
    ctx.dev_type = 1;
    ota_service_init(&ctx);
#endif

    EXAMPLE_TRACE("subdev login success: devid = %d\n", devid);
    return res;
}

int user_permit_join_event_handler(const char *product_key, const int time)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Product Key: %s, Time: %d", product_key, time);

    user_example_ctx->permit_join = 1;

    return 0;
}

void *user_dispatch_yield(void *args)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    while (user_example_ctx->g_user_dispatch_thread_running) {
        IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);
    }

    return NULL;
}

static int max_running_seconds = 0;
int linkkit_main(void *paras)
{
    int res = 0;
    uint64_t time_prev_sec = 0, time_now_sec = 0, time_begin_sec = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    iotx_linkkit_dev_meta_info_t master_meta_info;
    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));
#if defined(__UBUNTU_SDK_DEMO__)
    int                             argc = ((app_main_paras_t *)paras)->argc;
    char                          **argv = ((app_main_paras_t *)paras)->argv;

    if (argc > 1) {
        int tmp = atoi(argv[1]);

        if (tmp >= 60) {
            max_running_seconds = tmp;
            EXAMPLE_TRACE("set [max_running_seconds] = %d seconds\n", max_running_seconds);
        }
    }

    if (argc > 2) {
        if (strlen("auto") == strlen(argv[2]) &&
            memcmp("auto", argv[2], strlen(argv[2])) == 0) {
            user_example_ctx->auto_add_subdev = 1;
        }
    }
#endif

#if !defined(WIFI_PROVISION_ENABLED) || !defined(BUILD_AOS)
    set_iotx_info();
#endif

    user_example_ctx->subdev_index = -1;

    IOT_SetLogLevel(IOT_LOG_DEBUG);

    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    IOT_RegisterCallback(ITE_RAWDATA_ARRIVED, user_down_raw_data_arrived_event_handler);
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
#ifdef ALCS_ENABLED
    /*Only for local communication service(ALCS)*/
    IOT_RegisterCallback(ITE_PROPERTY_GET, user_property_get_event_handler);
#endif
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, user_trigger_event_reply_event_handler);
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);
    IOT_RegisterCallback(ITE_PERMIT_JOIN, user_permit_join_event_handler);
    IOT_RegisterCallback(ITE_CLOUD_ERROR, user_property_cloud_error_handler);
    IOT_RegisterCallback(ITE_EVENT_NOTIFY, user_event_notify_handler);
   
    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    HAL_GetProductKey(master_meta_info.product_key);
    HAL_GetDeviceName(master_meta_info.device_name);
    HAL_GetDeviceSecret(master_meta_info.device_secret);
    HAL_GetProductSecret(master_meta_info.product_secret);
	
	if((0 == strlen(master_meta_info.product_key))||(0 == strlen(master_meta_info.device_name))\
		||(0 == strlen(master_meta_info.device_secret))||(0 == strlen(master_meta_info.product_secret)))
	{
		EXAMPLE_TRACE("No device meta info found...\n");
		while (1) {
			aos_msleep(USER_EXAMPLE_YIELD_TIMEOUT_MS);
		}
	}

    /* Create Master Device Resources */
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
        return -1;
    }
#ifdef REGION_SINGAPORE
    int domain_type = IOTX_CLOUD_REGION_SINGAPORE;
#elif REGION_GERMANY
    int domain_type = IOTX_CLOUD_REGION_GERMANY;
#else /* Mainland(Shanghai) for default */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
#endif
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Choose Whether You Need Post Property/Event Reply */
    int post_event_reply = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);

    /* Start Connect Aliyun Server */
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
        return -1;
    }

    user_example_ctx->g_user_dispatch_thread_running = 1;
    res = HAL_ThreadCreate(&user_example_ctx->g_user_dispatch_thread, user_dispatch_yield, NULL, NULL, NULL);
    if (res < 0) {
        EXAMPLE_TRACE("HAL_ThreadCreate Failed\n");
        IOT_Linkkit_Close(user_example_ctx->master_devid);
        return -1;
    }

    time_begin_sec = user_update_sec();
    while (1) {
        HAL_SleepMs(200);

        time_now_sec = user_update_sec();
        if (time_prev_sec == time_now_sec) {
            continue;
        }
        if (max_running_seconds && (time_now_sec - time_begin_sec > max_running_seconds)) {
            EXAMPLE_TRACE("Example Run for Over %d Seconds, Break Loop!\n", max_running_seconds);
            break;
        }

        /* Add subdev */
        if (user_example_ctx->master_initialized && user_example_ctx->subdev_index >= 0 &&
            (user_example_ctx->auto_add_subdev == 1 || user_example_ctx->permit_join != 0)) {
            if (user_example_ctx->subdev_index < EXAMPLE_SUBDEV_ADD_NUM) {
                /* Add next subdev */
                if (example_add_subdev((iotx_linkkit_dev_meta_info_t *)&subdevArr[user_example_ctx->subdev_index]) == SUCCESS_RETURN) {
                    EXAMPLE_TRACE("subdev %s add succeed", subdevArr[user_example_ctx->subdev_index].device_name);
                } else {
                    EXAMPLE_TRACE("subdev %s add failed", subdevArr[user_example_ctx->subdev_index].device_name);
                }
                user_example_ctx->subdev_index++;
                user_example_ctx->permit_join = 0;
            }
        }

        /* Post Proprety Example */
        if (time_now_sec % 11 == 0 && user_master_dev_available()) {
            user_post_property();
            user_post_sub_property(user_example_ctx->subdev_index);
        }
        /* Post Event Example */
        if (time_now_sec % 17 == 0 && user_master_dev_available()) {
            user_post_event();
        }

        /* Device Info Update Example */
        if (time_now_sec % 23 == 0 && user_master_dev_available()) {
            user_deviceinfo_update();
        }

        /* Device Info Delete Example */
        if (time_now_sec % 29 == 0 && user_master_dev_available()) {
            user_deviceinfo_delete();
        }

        /* Post Raw Example */
        if (time_now_sec % 37 == 0 && user_master_dev_available()) {
            user_post_raw_data();
        }
        time_prev_sec = time_now_sec;
    }

    user_example_ctx->g_user_dispatch_thread_running = 0;
    IOT_Linkkit_Close(user_example_ctx->master_devid);
    HAL_ThreadDelete(user_example_ctx->g_user_dispatch_thread);

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_SetLogLevel(IOT_LOG_NONE);
    return 0;
}
#endif
