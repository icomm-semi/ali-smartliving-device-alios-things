#include "sys/backtrace.h"
#include "sys/flash.h"
#if defined(XIP_MODE)
#include "sys/xip.h"
#endif
#include "fsal.h"
#include "osal.h"
#include "wifinetstack.h"
#include "idmanage/pbuf.h"
#include "security/drv_security.h"
#include "phy/drv_phy.h"
#include "soc_defs.h"
#include "ieee80211_mgmt.h"
#include "ieee80211_mac.h"
#include "sta_func.h"
#include "wifi_api.h"
#include "netstack_def.h"
#include "netstack.h"
#include "uart/drv_uart.h"
#include "rf/rf_api.h"

void Cli_Task( void *args );

/**********************************************************/
SSV_FS fs_handle = NULL;

void ssvradio_init_task(void *pdata)
{
    printf("%s\n", __func__);
    WIFI_INIT();
    netstack_init(NULL);
    DUT_wifi_start(DUT_STA);
    
    OS_TaskDelete(NULL);
}

extern void drv_uart_init(void);
extern struct st_rf_table ssv_rf_table;
void APP_Init(void)
{
#ifdef XIP_MODE
	xip_init();
	xip_enter();
#endif
	drv_uart_init();
    drv_uart_set_fifo(UART_INT_RXFIFO_TRGLVL_1, 0x0);
	drv_uart_set_format(115200, UART_WORD_LEN_8, UART_STOP_BIT_1, UART_PARITY_DISABLE);

	OS_Init();
	OS_StatInit();
	OS_MemInit();
	OS_PsramInit();

        load_rf_table_from_flash();
    if(ssv_rf_table.boot_flag == 0xFF)
    {
        build_default_rf_table(&ssv_rf_table);
        load_rf_table_to_mac(&ssv_rf_table);
        save_rf_table_to_flash();
        dump_rf_table();
    }
    else
    {
        load_rf_table_to_mac(&ssv_rf_table);
        dump_rf_table();
    }

	fs_handle = FS_init();
	if(fs_handle)
	{
		FS_remove_prevota(fs_handle);
	}

    OS_TaskCreate(Cli_Task, "cli", 512, NULL, OS_TASK_PRIO3, NULL);

    OS_TaskCreate(ssvradio_init_task, "ssvradio_init", 256, NULL, OS_TASK_PRIO3, NULL);

    printf("[%s][%d] string\n", __func__, __LINE__);

    OS_StartScheduler();
}

#define M_GPIO_DEFAULT          (0)
#define M_GPIO_USER_DEFINED     (1)

// this will increase current.
int lowpower_sleep_gpio_hook() {
    // do your gpio setting
    //return M_GPIO_USER_DEFINED;
    // use default gpio setting.
    return M_GPIO_DEFAULT;
}

// this will increase current.
int lowpower_dormant_gpio_hook() {
    // do your gpio setting
    //return M_GPIO_USER_DEFINED;
    // use default gpio setting.
    return M_GPIO_DEFAULT;
}

void lowpower_pre_sleep_hook() {
    // do nothing
}

void lowpower_post_sleep_hook() {
    // do nothing
}

void vAssertCalled( const char *func, int line )
{
    printf("<!!!OS Assert!!!> func = %s, line=%d\n", func, line);
    print_callstack();
    while(1);
}

