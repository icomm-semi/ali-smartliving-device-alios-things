#ifndef DRV_TIM_TIMER_H
#define DRV_TIM_TIMER_H

#include <sys/intc.h>

void drv_tim_timer_init();
void drv_tim_timer_register_irq_handler(isr_func local_tm_irq);
void drv_tim_timer_enable(int time);
void drv_tim_timer_disable();
void drv_tim_timer_clear_isr();
#endif /* end of include guard: DRV_TIM_TIMER_H */
