#include <k_api.h>
#include "mcu_def.h"
#include "soc_defs.h"
#include "sys/systick.h"
#include "sys/intc.h"

void systick_isr()
{
	intc_irq_clean(IRQ_UTIMER_0);

    if (systick_get_period() != systick_get_timer_period()) {
        systick_stop();
        systick_start();
    }

    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}
