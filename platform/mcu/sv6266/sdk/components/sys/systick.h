#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include <stdint.h>
#include "attrs.h"

enum { 
    SYSTICK_PRESCALE_ENABLE = 1,
    SYSTICK_PRESCALE_DISABLE = 0
};        

#define SYSTICK_PRESCALE 5

void systick_set_period(uint32_t period);
void systick_start(void);
void systick_stop(void);
uint32_t systick_get_period();
uint32_t systick_get_timer_period();
uint32_t systick_get_remain_count(void);
uint32_t systick_get_irq_status();

void systick_set_prescale_state(uint8_t enable);
void systick_set_prescale(void);
uint8_t systick_get_prescale_state(void);

uint32_t systick_get_period();

#endif
