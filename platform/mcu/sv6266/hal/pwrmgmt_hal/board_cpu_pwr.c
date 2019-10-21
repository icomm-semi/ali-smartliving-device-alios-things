/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
DESCRIPTION
This library provides the support for the STM32L496G-DISCOVERY
CPU power state control.

CPU power management:
provides low-level interface for setting CPU C-states.
provides low-level interface for setting CPU P-states.
*/

#include <k_api.h>

#if (AOS_COMP_PWRMGMT > 0)

#include <pwrmgmt_api.h>
#include <cpu_pwr_hal_lib.h>
#include <pwrmgmt_debug.h>
#include <cpu_tickless.h>
#include <lowpower.h>
#define REG32(reg) (*(uint32_t volatile *)(reg))

extern one_shot_timer_t rtc_one_shot;
extern one_shot_timer_t standby_rtc_one_shot;
static cpu_pwr_t cpu_pwr_node_core_0;

uint32_t g_sleep_us;

/**
 * board_cpu_c_state_set - program CPU into Cx idle state
 *
 * RUN Context: could be called from ISR context or task context.
 *
 * SMP Consider: STM32L496G-DISCOVERY do not support SMP, so only UP is enough.
 *
 * @return  PWR_OK or PWR_ERR when failed.
 */
static pwr_status_t board_cpu_c_state_set(uint32_t cpuCState, int master)
{
#if (PWRMGMT_CONFIG_LOG_ENTERSLEEP > 0)
     static sys_time_t last_log_entersleep = 0;
#endif
    switch (cpuCState) {
        case CPU_CSTATE_C0:

            //printf("[%s] %s\n", __func__, "CPU_CSTATE_C0");
            if (master) {
                /*
                 * do something needed when CPU waked up from C1 or higher
                 * Cx state.
                 */
            }
            break;

        case CPU_CSTATE_C1:
#if (PWRMGMT_CONFIG_LOG_ENTERSLEEP > 0)
            if (krhino_sys_tick_get() > (last_log_entersleep + RHINO_CONFIG_TICKS_PER_SECOND)) {
                last_log_entersleep = krhino_sys_tick_get();
                printf("enter standby\r\n");
            }
#endif
            sys_clock_slow_wfi();
            break;

        case CPU_CSTATE_C2:
            /* put CPU into C2 state, for ARM we can call WFI instruction
               to put CPU into C2 state. */
            //printf("[%s] %s, %d\n", __func__, "CPU_CSTATE_C2", g_sleep_us);
            PWR_DBG(DBG_INFO, "enter C2\n");

            // save hw timer
            sys_save_timer();

            // check hw timer remain time.
            int32_t hw_tmr_remain_tick_count = sys_timer_min_time();
            if (g_sleep_us > hw_tmr_remain_tick_count) {
                g_sleep_us = hw_tmr_remain_tick_count;
            }

            // system sleep.
            if (g_sleep_us < (1800)) {
                //if (hw_tmr_remain_tick_count > 0) {
                //    g_sleep_us = 1;
                //}
                //else {
                //    g_sleep_us = 0;
                //}
                //
                g_sleep_us = 1;
            } else {
#if 1
                // disable timer clock.
                uint32_t clk = sys_stop_clk();

                g_sleep_us = sys_sleep(g_sleep_us);

                // restore timer clock.
                sys_resume_clk(clk);

                // restore hw timer.
                sys_timer_restore_time(g_sleep_us);
#else
                system_delay(system_us2delaytick(g_sleep_us));
#endif
            }



#if (PWRMGMT_CONFIG_LOG_ENTERSLEEP > 0)
            if (krhino_sys_tick_get() > (last_log_entersleep + RHINO_CONFIG_TICKS_PER_SECOND)) {
                last_log_entersleep = krhino_sys_tick_get();
                printf("enter sleep %d us\r\n", (uint32_t) g_sleep_us);
            }
#endif

            // add code to set cpu enter sleep
            PWR_DBG(DBG_INFO, "exit C2\n");
            break;

        default:
            printf("[%s] default\n", __func__);
            PWR_DBG(DBG_ERR, "invalid C state: C%d\n", cpuCState);
            break;
    }

    return PWR_OK;
}

/**
 * board_cpu_pwr_init() is called by HAL lib to
 * init board powr manage configure.
 *
 * RUN Context: could be called from task context only, ISR context is not
 * supported.
 *
 * SMP Consider: STM32L496G-DISCOVERY do not support SMP, so only UP is enough.
 *
 * @return  PWR_OK or PWR_ERR when failed.
 */
pwr_status_t board_cpu_pwr_init(void)
{
    cpu_pwr_t    *pCpuNode = NULL;
    pwr_status_t  retVal   = PWR_OK;
    uint32_t      cpuIndex = 0;    /* 0 for UP */

    pCpuNode = &cpu_pwr_node_core_0;
    retVal   = cpu_pwr_node_init_static("core", 0, pCpuNode);
    if (retVal != PWR_OK) {
        return PWR_ERR;
    }

    /* record this node */
    retVal = cpu_pwr_node_record(pCpuNode, cpuIndex);
    if (retVal == PWR_ERR) {
        return PWR_ERR;
    }

    /*
     * According reference manual of STM32L496G-DISCOVERY
     *
     * C0 - RUN,  Power supplies are on,all clocks are on.
     * C1 - Sleep mode, CPU clock off, all peripherals including
     *      Cortex®-M4 core peripherals such as NVIC, SysTick, etc. can run
     *      and wake up the CPU when an interrupt or an event occurs.
     */

    retVal = cpu_pwr_c_method_set(cpuIndex, board_cpu_c_state_set);
    if (retVal == PWR_ERR) {
        return PWR_ERR;
    }

#define HAVE_RTC
    /* save support C status bitset : C0,C1 */
#if defined(HAVE_RTC)
    cpu_pwr_c_state_capability_set(cpuIndex, CPU_STATE_BIT(CPU_CSTATE_C0)
                                   | CPU_STATE_BIT(CPU_CSTATE_C1) | CPU_STATE_BIT(CPU_CSTATE_C2));
#else
    cpu_pwr_c_state_capability_set(cpuIndex, CPU_STATE_BIT(CPU_CSTATE_C0)
                                   | CPU_STATE_BIT(CPU_CSTATE_C1));
#endif
    if (retVal == PWR_ERR) {
        return PWR_ERR;
    }

    /*
     * According reference manual of STM32L496G-DISCOVERY,
     * the wakeup latency of Cx is:
     * resume from C1 (Low Power mode)       : immediate
     */
    cpu_pwr_c_state_latency_save(cpuIndex, CPU_CSTATE_C0, 0);
    cpu_pwr_c_state_latency_save(cpuIndex, CPU_CSTATE_C1, 0);
    cpu_pwr_c_state_latency_save(cpuIndex, CPU_CSTATE_C2, 2000);

    tickless_one_shot_timer_save(CPU_CSTATE_C1, &standby_rtc_one_shot);
    tickless_one_shot_timer_save(CPU_CSTATE_C2, &rtc_one_shot);

    /*
    Tell the CPU PWR MGMT module which C state is supported with
    tickless function through tickless_c_states_add(c_state_x).
    */
#if defined(HAVE_RTC)
    tickless_c_states_add(CPU_STATE_BIT(CPU_CSTATE_C0)
                          | CPU_STATE_BIT(CPU_CSTATE_C1) | CPU_STATE_BIT(CPU_CSTATE_C2                          ));
#else
    tickless_c_states_add(CPU_STATE_BIT(CPU_CSTATE_C0)
                          | CPU_STATE_BIT(CPU_CSTATE_C1) );
#endif
#if RHINO_CONFIG_CPU_PWR_SHOW
    cpu_pwr_info_show();
    cpu_pwr_state_show();
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
    lowpower_mode(E_LOW_POWER_SLEEP);
#endif

    return retVal;
}

int pwrmgmt_suspend_lowpower() {
    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
    //add code to suspend cpu sleep
    RHINO_CRITICAL_EXIT_SCHED();
#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
    //add code to suspend wifi lowpower
    //printf("[%s]!!!!!!!!!!!\n", __func__);
    lowpower_mode(E_LOW_POWER_ACTIVE);
#endif
    return 1;
}

int pwrmgmt_resume_lowpower() {
    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
    //add code to resume cpu sleep
    RHINO_CRITICAL_EXIT_SCHED();
#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
    //add code to resume wifi lowpower
    //printf("[%s]!!!!!!!!!!!\n", __func__);
    lowpower_mode(E_LOW_POWER_SLEEP);
#endif
    return 1;
}
#endif /* AOS_COMP_PWRMGMT */
