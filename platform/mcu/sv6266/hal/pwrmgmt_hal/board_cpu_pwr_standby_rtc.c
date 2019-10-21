/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
 * This file supplied RTC one-shot start/stop services for CPU tickless
 * module, verifyied on STM32L496-DISCOVERY with C3/C4 mode.
 * C3: stop mode.
 * C4: standby mode.
 */
#include <k_api.h>

#if (AOS_COMP_PWRMGMT > 0)
#include <stdint.h>
#include <stdbool.h>

#include <cpu_tickless.h>
#include <osal.h>
#include <hal_tm.h>

#define RTC_HW_TIMER_ID        TM_TU1_US
#define RTC_HW_TIMER_MAX_COUNT 65535
#define RTC_HW_TIMER_FREQ      1000000
#define MSEC_PER_SYSTICK       (1000000 / RHINO_CONFIG_TICKS_PER_SECOND)
#define SYSTICK_FREQ ((uint64_t)1000000)

static pwr_status_t rtc_init(void);
static uint32_t     rtc_one_shot_max_msec(void);
static pwr_status_t rtc_one_shot_start(uint64_t planUs);
static pwr_status_t rtc_one_shot_stop(uint64_t *pPassedUs);

one_shot_timer_t standby_rtc_one_shot = {
    rtc_init,
    rtc_one_shot_max_msec,
    rtc_one_shot_start,
    rtc_one_shot_stop,
};

static uint32_t start_count = 0;
extern uint32_t systick_remain_us;
extern uint32_t systick_passed_us;

static void tmr_irq_handler (uint32_t irq_num)
{
//    printf ("%s:%d: In function: '%s': tmr%ld interrupt occurred\n", __FILENAME__, __LINE__ - 1, __func__, irq_num - IRQ_UTIMER_0);
    drv_tmr_clear_interrupt_status (irq_num - IRQ_UTIMER_0);
}

static pwr_status_t rtc_init(void)
{
    drv_tmr_init(RTC_HW_TIMER_ID);
    drv_tmr_sw_rst(RTC_HW_TIMER_ID);
    drv_tmr_register_irq_handler(RTC_HW_TIMER_ID, tmr_irq_handler);
    return PWR_OK;
}

static pwr_status_t rtc_one_shot_start(uint64_t planUs)
{
    if (planUs <= systick_passed_us) {
        return PWR_ERR;
    }
    planUs = planUs - systick_passed_us;
    if (drv_tmr_get_interrupt_status(RTC_HW_TIMER_ID)) {
        drv_tmr_clear_interrupt_status(RTC_HW_TIMER_ID);
    }

    start_count = planUs * (RTC_HW_TIMER_FREQ / (uint64_t)1000000);

    drv_tmr_enable(RTC_HW_TIMER_ID, TM_MODE_AUTO_RELOAD, start_count);
    return PWR_OK;
}

static pwr_status_t rtc_one_shot_stop(uint64_t *pPassedUs)
{
    uint32_t count        = 0;
    uint64_t passed_timer = 0;

    count = drv_tmr_get_current_count(RTC_HW_TIMER_ID);

    if ((count != 0) && (drv_tmr_get_interrupt_status(RTC_HW_TIMER_ID))) {
        count = start_count - count + start_count;
    } else if (start_count >= count){
        count = start_count - count;
    } else {
        printf("error : start_count = %d, count = %d\r\n", start_count, count);
        count = 0;
    }

    passed_timer      = count * ((uint64_t)1000000 / RTC_HW_TIMER_FREQ) + systick_passed_us;
    *pPassedUs        = passed_timer / MSEC_PER_SYSTICK * MSEC_PER_SYSTICK;
    systick_remain_us = SYSTICK_FREQ / RHINO_CONFIG_TICKS_PER_SECOND - passed_timer % MSEC_PER_SYSTICK;

    drv_tmr_disable(RTC_HW_TIMER_ID);

    //printf("[%s]!sleep_ed %lld, remian %d\n", __func__, *pPassedUs, systick_remain_us);
    return PWR_OK;
}

static uint32_t rtc_one_shot_max_msec(void)
{
    return RTC_HW_TIMER_MAX_COUNT * 1000 / RTC_HW_TIMER_FREQ;
}

#endif /* AOS_COMP_PWRMGMT */
