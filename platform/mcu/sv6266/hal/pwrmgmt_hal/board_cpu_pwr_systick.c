/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
DESCRIPTION
This file provides two fundtions systick_suspend()/systick_resume()
which is used by cpu tickless module to suspend/resume system tick
interrupt.

Differrent board may has different way to suspend/resume system tick
interrupt, please reference your board/soc user manual to find the
detail for how to implement these two functions.
*/

#include <k_api.h>
#include <drv_tmr.h>
#include <soc_defs.h>

#if (AOS_COMP_PWRMGMT > 0)
#define SYSTICK_FREQ ((uint64_t)1000000)

uint32_t systick_remain_us = 0;
uint32_t systick_passed_us = 0;

void systick_suspend(void)
{
    systick_remain_us = systick_get_remain_count();
    systick_passed_us = (SYSTICK_FREQ / RHINO_CONFIG_TICKS_PER_SECOND - systick_remain_us) * (1000000 / SYSTICK_FREQ);
  //  printf("suspend remain %d\r\n", systick_remain_us);
    intc_irq_disable(IRQ_SYSTICK);
    hal_tm_deinit(TM_TU0_US);
}

void systick_resume(void)
{
    hal_tm_init(TM_TU0_US);
    systick_stop();
   // printf("suspend resume %d\r\n", systick_remain_us);
    systick_set_period(systick_remain_us * (SYSTICK_FREQ / 1000000));
    systick_start();
    systick_set_period(SYSTICK_FREQ / RHINO_CONFIG_TICKS_PER_SECOND);
    intc_irq_enable(IRQ_SYSTICK);
}

#endif /* AOS_COMP_PWRMGMT */
