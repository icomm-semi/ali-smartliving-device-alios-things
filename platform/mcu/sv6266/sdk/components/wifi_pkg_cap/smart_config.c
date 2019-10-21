#include "smart_config.h"
#include "osal.h"
#include <string.h>

#define M_SNIFFER_MSGQ_MAX_SIZE     (4)

enum {
    E_SNIFFER_MSG_PKG   = 0,
    E_SNIFFER_MSG_TERM  = 1,
};

typedef void (*_sniffer_cb_fn)(packetinfo *);

//struct _sniffer_task_param
//{
//    _sniffer_cb_fn sniffer_cb;
//};

static OsTimer ch_os_timer = NULL;
void (*channel_cb)(void) = NULL;


static OsTaskHandle g_sniffer_task_handler = NULL;
static OsMsgQ g_sniffer_msgq = NULL;
static OsEvent g_sniffer_join = NULL;
static _sniffer_cb_fn g_sniffer_ptr = NULL;

static void free_pkt(packetinfo *pkt);

static void sniffer_task(void *param)
{
    g_sniffer_ptr = (_sniffer_cb_fn) param;
    packetinfo *pkt = NULL;
    u8 sniffer_loop = 1;

    if (g_sniffer_ptr == NULL)
    {
        return;
    }

    if (g_sniffer_msgq == NULL)
    {
        printf("[%s] Please check the msgq\n", __func__);
        return;
    }
    else
    {
        // why msgq already alloc??
        //printf("[%s] Please check the msgq owner\n", __func__);
    }
    while(sniffer_loop)
    {
        OsMsgQEntry msg;
        if (OS_MsgQDequeue(g_sniffer_msgq, &msg, portMAX_DELAY) == OS_SUCCESS)
        {
            switch (msg.MsgCmd)
            {
                case E_SNIFFER_MSG_TERM:
                    printf("[%s] E_SNIFFER_MSG_TERM\n", __func__);
                    sniffer_loop = 0;
                    break;
                case E_SNIFFER_MSG_PKG:
                    pkt = (packetinfo *)msg.MsgData;
                    if (g_sniffer_ptr != NULL)
                    {
                        g_sniffer_ptr(pkt);
                    }
                    free_pkt(pkt);
                    break;
                default:
                    printf("[%s] get a error message\n", __func__);
                    break;
            }
        }
    }

    OS_EventSet(g_sniffer_join);
    OS_TaskDelete(NULL);
}

static void free_pkt(packetinfo *pkt)
{
    OS_MemFree(pkt->data);
    OS_MemFree(pkt);
}

static packetinfo *dup_pkt(packetinfo *pkt)
{
    packetinfo *ptr = NULL;
    ptr = (packetinfo *)OS_MemAlloc(sizeof(packetinfo));
    if (ptr == NULL)
    {
        return NULL;
    }
    memcpy(ptr, pkt, sizeof(packetinfo));
    ptr->data = (u8 *)OS_MemAlloc(pkt->len);
    if (ptr->data == NULL)
    {
        OS_MemFree(ptr);
        return NULL;
    }
    memcpy(ptr->data, pkt->data, pkt->len);
    return ptr;
}

static void sniffer_cb(packetinfo *pkt)
{
    if ((g_sniffer_task_handler != NULL) && (g_sniffer_msgq != NULL))
    {
        packetinfo *ptr = NULL;

        ptr = dup_pkt(pkt);
        if (ptr != NULL) {
            OsMsgQEntry msg;
            msg.MsgCmd = E_SNIFFER_MSG_PKG;
            msg.MsgData = ptr;
            if (OS_MsgQEnqueue(g_sniffer_msgq, &msg) != OS_SUCCESS)
            {
                // send msg failed. free memory.
                free_pkt(ptr);
                return;
            }
        } else {
            // memory not enough.
        }
    }
}

u8 attach_sniffer_cb(SNIFFER_RECVINDEX recv_filter, void (*cb_fn)(packetinfo *), u16 stack_size)
{
    printf("%s\n", __func__);

    if (g_sniffer_msgq == NULL)
    {
        if (OS_MsgQCreate(&g_sniffer_msgq, M_SNIFFER_MSGQ_MAX_SIZE) != OS_SUCCESS)
        {
            printf("[%s] create sniffer msg queue failed, close task\n", __func__);
            return SC_NG;
        }
    } else
    {
        printf("[%s] msgq not release, something wrong!, mem leak?!!\n", __func__);
    }

    if (g_sniffer_join == NULL)
    {
        if (OS_EventCreate(&g_sniffer_join) != OS_SUCCESS)
        {
            printf("[%s] create sniffer event failed, close task\n", __func__);
            OS_MsgQDelete(g_sniffer_msgq);
            return SC_NG;
        }
    } else
    {
        printf("[%s] event not release, something wrong!, mem leak?!!\n", __func__);
    }

    if (OS_TaskCreate(sniffer_task, "sniffer_task", stack_size, (void *)cb_fn, OS_TASK_PRIO3, &g_sniffer_task_handler) == 0) {
        printf("[%s] create sniffer task failed\n", __func__);
        OS_MsgQDelete(g_sniffer_msgq);
        g_sniffer_msgq = NULL;
        OS_EventDelete(g_sniffer_join);
        g_sniffer_join = NULL;
        return SC_NG;
    }
    set_sniffer_config(recv_filter, sniffer_cb);
    return SC_OK; 
}

static void stop_sniffer_task(void *param)
{
    if (g_sniffer_msgq != NULL)
    {
        OsMsgQEntry msg;
        while (true)
        {
            msg.MsgCmd = E_SNIFFER_MSG_TERM;
            msg.MsgData = NULL;
            if (OS_MsgQEnqueue(g_sniffer_msgq, &msg) == OS_SUCCESS)
            {
                break;
            }
        }
        while (OS_EventWait(g_sniffer_join, OS_MAX_WAITTIME) != OS_SUCCESS);
        OS_EventDelete(g_sniffer_join);
        g_sniffer_join = NULL;
        while (OS_MsgQDequeue(g_sniffer_msgq, &msg, 1) == OS_SUCCESS)
        {
            switch (msg.MsgCmd)
            {
                case E_SNIFFER_MSG_PKG:
                    free_pkt(msg.MsgData);
                    break;
            }
        }
        OS_MsgQDelete(g_sniffer_msgq);
        g_sniffer_msgq = NULL;
    }
    else
    {
    }
    OS_TaskDelete(NULL);
}

u8 deattach_sniffer_cb(void)
{
    g_sniffer_ptr = NULL;
    printf("%s\n", __func__);
    set_sniffer_config(RECV_DEFAULT, NULL);

    OS_TaskCreate(stop_sniffer_task, "disable_sniffer", 256, NULL, OS_TASK_PRIO2, NULL);
    // aMH terminal sniffer_task. delete msgq

    //OS_TaskDelete(g_sniffer_task_handler);
    return SC_OK;    
}

void start_sniffer_mode(void)
{
    printf("%s\n", __func__);

    //if WIFI_OPMAODE != DUT_DONE, it would call DUT_wifi_stop first.
    DUT_wifi_start(DUT_SNIFFER);
}

void stop_sniffer_mode(void)
{
    printf("%s\n", __func__);
    clear_rxque_buffer();
    DUT_wifi_stop();
}

void ch_change_hdlr(void)
{
    static u8 channel = 0;
    static u8 ch_lists[] = {1, 4, 7, 10, 13, 2, 5, 8, 11, 3, 6, 9, 12};
    channel = ((channel + 1) % 13);
    set_channel(ch_lists[channel]);
    if (channel_cb) {
        channel_cb();    
    }
    OS_TimerStart(ch_os_timer);
}

inline void set_channel(u8 ch)
{
    gwifistatus.run_channel = ch; 
    wifi_set_channel((u32)ch, NL80211_CHAN_HT20);
}

inline u8 get_current_channel(void)
{
    return gwifistatus.run_channel;    
}

void attach_channel_change_cb(void (*ch_cb)(void))
{
    channel_cb = ch_cb;
}

void deattach_channel_change_cb(void)
{
    channel_cb = NULL;
}

u8 auto_ch_switch_start(u8 time_interval)
{
    printf("%s\n", __func__);
    if (ch_os_timer != NULL) {
        printf("ch_os_timer already running\n");
        return SC_NG;
    }

    if( OS_TimerCreate(&ch_os_timer, time_interval, (u8)FALSE, NULL, (OsTimerHandler)ch_change_hdlr) == OS_FAILED) {
        printf("OS_TimerCreate NG\n");
        return SC_NG;
    }

    OS_TimerStart(ch_os_timer);
    return SC_OK;
}

u8 auto_ch_switch_stop(void)
{
    printf("%s\n", __func__);
    if (channel_cb)
        channel_cb = NULL;
    if (ch_os_timer) {
        OS_TimerStop(ch_os_timer);
        ch_os_timer = NULL;
    }
    return SC_OK;
}

