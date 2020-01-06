/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>

#include "sys/xip.h"
#include "sys/dbgcon.h"
#include "osal.h"
#include "wifinetstack.h"
#include "idmanage/pbuf.h"
#include "security/drv_security.h"
//#include "phy/drv_phy.h"
#include "soc_defs.h"
#include "ieee80211_mgmt.h"
#include "ieee80211_mac.h"
#include "sta_func.h"
#include "wifi_api.h"
#include "netstack.h"
#include "netstack_def.h"
#include "wifi_api.h"
#include "rf/rf_api.h"
#include "wdt/drv_wdt.h"
#include "gpio/drv_gpio.h"
#include "drv_uart.h"
#include "hal/soc/soc.h"
#include <cJSON.h>

#if defined (VCALL_RHINO)
#if defined (CONFIG_AOS_CLI)
#include "dumpsys.h"
#endif
#endif
#define AOS_START_STACK (2048+512)

ktask_t *g_radio_init;
void uart_rx_isr(void);
extern int write_reg_rf_table_ex(struct st_rf_table * p_table);
extern struct st_rf_table ssv_rf_table;

static void ssvradio_init_task(void *pdata)
{
    PBUF_Init();
    NETSTACK_RADIO.init();    
    drv_sec_init();
	write_reg_rf_table_ex(&ssv_rf_table);
    netstack_init(NULL);
    OS_TaskDelete(NULL);
}

extern uint32_t SAVED_PC;
extern uint32_t SAVED_LP;
extern void dump_ir();
static void aos_wdt_process() {
	printf("LP:%xh\n", SAVED_LP);
	printf("IPC:%xh\n", SAVED_PC);
    //ktask_t *cur = krhino_cur_task_get();
    ktask_t *task = g_active_task[0];
    printf("TP:%xh\n", task);
    printf("TN:%s\n", task->task_name);
    printf("g_intrpt_nested_level[0]=%d\n", g_intrpt_nested_level[0]);
    dump_ir();
#if defined (VCALL_RHINO)
#if defined (CONFIG_AOS_CLI)
    dumpsys_task_func(NULL, 0, 1);
#endif
#endif
}

#if defined(CONFIG_ENABLE_WDT)
static void wdt_task(void *pdata)
{
    drv_wdt_init();
    printf("start wdt\n");
    drv_wdt_enable(SYS_WDT, 50000);
    drv_wdt_register_isr(SYS_WDT, 255, aos_wdt_process);
    while(1)
    {
        OS_MsDelay(3*1000);
    //    printf("kick\n");
        drv_wdt_kick(SYS_WDT);
    }
    OS_TaskDelete(NULL);
}
#endif

ktask_t *g_aos_init;
static kinit_t kinit;
static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

extern void hw_start_hal(void);
extern void board_init();
static void system_init(void)
{
    board_init();
    hw_start_hal();
    var_init();
    aos_kernel_init(&kinit);
}

#define FLASH_USER_MAP	(0x30000000+0x6000)
typedef struct
{
	char *key;
	char *kv_str;
	char *kv_val;
}user_data_info_t;

static user_data_info_t user_data_info[]=
{
	{"PRODUCT_KEY",		"linkkit_product_key",	NULL},
	{"PRODUCT_SECRET",	"linkkit_product_secret",	NULL},
	{"DEVICE_NAME",		"linkkit_device_name",	NULL},
	{"DEVICE_SECRET",	"linkkit_device_secret",	NULL},
	{"PRODUCT_ID",		"linkkit_product_id",	NULL},		//only for ble awss	
};


static int8_t user_data_init()
{
	cJSON *cjson_hd = NULL;
	cJSON *wifi_cfg_hd = NULL;
	cJSON *obj_ptr = NULL;
	char *tmpchar = NULL;
	char soclicense[100];
	int i;

	printf("user header val 0x%x\n",*(volatile uint32_t *) FLASH_USER_MAP);
	if(*(volatile uint32_t *) FLASH_USER_MAP != 0x55aa55aa)
	{
		printf("user_data not set\n");
		return -1;
	}

	cjson_hd = cJSON_Parse((int8_t *)(FLASH_USER_MAP + sizeof(uint32_t)));
	if(cjson_hd == NULL)
	{
		printf("user_data cjson error\n");
		return -1;
	}

//set MAC address info
	wifi_cfg_hd = wifi_cfg_init();

	obj_ptr = cJSON_GetObjectItem(cjson_hd, "MAC1");
	if(obj_ptr != NULL)
	{
		tmpchar = obj_ptr->valuestring;
		printf("%s set MAC1 %s\n",__func__,tmpchar);
		wifi_cfg_replace_mem_addr1(wifi_cfg_hd,tmpchar);
		tmpchar = NULL;
	}

	obj_ptr = cJSON_GetObjectItem(cjson_hd, "MAC2");
	if(obj_ptr != NULL)
	{
		tmpchar = obj_ptr->valuestring;
		printf("%s set MAC2 %s\n",__func__,tmpchar);
		wifi_cfg_replace_mem_addr2(wifi_cfg_hd,tmpchar);
		tmpchar = NULL;
	}
	wifi_cfg_write_cfg(wifi_cfg_hd);

	wifi_cfg_deinit(wifi_cfg_hd);

//get -set alios product info
	for(i=0; i<sizeof(user_data_info)/sizeof(user_data_info[0]); i++)
	{
		obj_ptr = cJSON_GetObjectItem(cjson_hd, user_data_info[i].key);
		if(obj_ptr != NULL)
		{
			tmpchar = obj_ptr->valuestring;
			aos_kv_set(user_data_info[i].kv_str, tmpchar, strlen(tmpchar) + 1, 1);
			user_data_info[i].kv_val = tmpchar;
			printf("user data %s:%s\n",user_data_info[i].kv_str,tmpchar);
		}
	}

	if(user_data_info[0].kv_val == NULL || \
		user_data_info[1].kv_val == NULL || \
		user_data_info[2].kv_val == NULL || \
		user_data_info[3].kv_val == NULL )
	{
		return 0;
	}
	memset(soclicense, 0, sizeof(soclicense));
	snprintf(soclicense, sizeof(soclicense),"%s,%s,%s,%s",\
		user_data_info[0].kv_val,\
		user_data_info[1].kv_val,
		user_data_info[2].kv_val,
		user_data_info[3].kv_val);
	
	aos_kv_set("SOCLICENSE",soclicense,strlen(soclicense)+1,1);
	printf("user data SOCLICENSE set: %s\n",soclicense);
	cJSON_Delete(cjson_hd);

	return 0;
}

static void burn_info_process_task(void *pdata)
{
	uint32_t val = 0;
	OS_MsDelay(1*1000);
	if(user_data_init() == 0)
	{
//clear flash header
		cfg_user_write_cfg(&val,sizeof(uint32_t));
		printf("Reboot to Reflash burn InFo......\n");
		drv_wdt_init();
		drv_wdt_enable(SYS_WDT, 500);
		while(1);
	}
	OS_TaskDelete(NULL);

}

static void do_awss_reset();
void isr_gpio_12()
{
    drv_gpio_intc_clear(GPIO_12);
    //aos_schedule_call(do_awss_reset, NULL);
    //REG32(0xc0000c00) = '?';
//    REG32(0xc0000c00) = '1';
//    REG32(0xc0000c00) = '2';
//    REG32(0xc0000c00) = '\n';
}
extern void do_awss_active();
void isr_gpio_11()
{
    drv_gpio_intc_clear(GPIO_11);
    //aos_schedule_call(do_awss_active, NULL);
    //REG32(0xc0000c00) = '!';
//    REG32(0xc0000c00) = '1';
//    REG32(0xc0000c00) = '1';
//    REG32(0xc0000c00) = '\n';
}


extern uart_dev_t uart_0;

static void app_start(void)
{
    xip_init();
    xip_enter();
    flash_init();

    //load_rf_table_from_flash();
    //write_reg_rf_table();
    
    //dbgcon_init(UART_SPR_BAUD_921600);
    drv_uart_init();
    drv_uart_set_fifo(UART_INT_RXFIFO_TRGLVL_1, 0x0);
    drv_uart_set_format(921600, UART_WORD_LEN_8, UART_STOP_BIT_1, UART_PARITY_DISABLE);
    //drv_uart_register_isr(UART_DATA_RDY_IE, uart_rx_isr);
    
    OS_Init();
    OS_MemInit();
    hal_uart_init (&uart_0);
    //OS_PsramInit();

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

    //drv_gpio_set_dir(GPIO_12, GPIO_DIR_IN);
    //drv_gpio_set_dir(GPIO_11, GPIO_DIR_IN);
    //drv_gpio_set_pull(GPIO_12, GPIO_PULL_UP);
    //drv_gpio_set_pull(GPIO_11, GPIO_PULL_UP);

    //drv_gpio_intc_trigger_mode(GPIO_12, GPIO_INTC_FALLING_EDGE);
    //drv_gpio_intc_trigger_mode(GPIO_11, GPIO_INTC_FALLING_EDGE);

    //drv_gpio_register_isr(GPIO_12, isr_gpio_12);
    //drv_gpio_register_isr(GPIO_11, isr_gpio_11);
    
    OS_TaskCreate(ssvradio_init_task, "ssvradio_init", 512, NULL, 1, NULL);
    OS_TaskCreate(burn_info_process_task, "burn_info_process_task", 512, NULL, 1, NULL);
#if defined(CONFIG_ENABLE_WDT)
    OS_TaskCreate(wdt_task, "wdt", 256+128, NULL, 15, NULL);
#endif
    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)system_init, 1);
    
    OS_StartScheduler();
}

void APP_Init(void)
{
    app_start();
}

#define M_GPIO_DEFAULT          (0)
#define M_GPIO_USER_DEFINED     (1)

// this will increase current.
int lowpower_sleep_gpio_hook() ATTRIBUTE_SECTION_FAST_TEXT;
int lowpower_sleep_gpio_hook() {
    // do your gpio setting
    //return M_GPIO_USER_DEFINED;
    // use default gpio setting.
    //return M_GPIO_USER_DEFINED;
    return M_GPIO_DEFAULT;
}

// this will increase current.
int lowpower_dormant_gpio_hook() ATTRIBUTE_SECTION_FAST_TEXT;
int lowpower_dormant_gpio_hook() {
    // do your gpio setting
    //return M_GPIO_USER_DEFINED;
    // use default gpio setting.
    return M_GPIO_DEFAULT;
}

void lowpower_pre_sleep_hook() ATTRIBUTE_SECTION_FAST_TEXT;
void lowpower_pre_sleep_hook() {
    // do nothing
}

void lowpower_post_sleep_hook() ATTRIBUTE_SECTION_FAST_TEXT;
void lowpower_post_sleep_hook() {
    // do nothing
}
