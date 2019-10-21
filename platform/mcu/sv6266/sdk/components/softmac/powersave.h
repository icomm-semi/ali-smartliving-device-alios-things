#ifndef POWERSAVE_H_
#define POWERSAVE_H_
#include <attrs.h>

typedef enum {
    NULL_SUCC_ACTIVE    = 0x0,
    NULL_SUCC_POWERSAVE,
    NULL_FAIL,
}NULL_TXRSP;

struct dtim_data
{
    s32 rsdiff;
    u8 dtim_cnt;
    u8 dtim_peroid;
    u8 keepdata;
};

typedef enum t_MCU_CFG
{
    MCU_ON = 0,
    MCU_OFF,
} MCU_CFG;

typedef enum t_RFPHY_STA
{
    RFPHY_ON = 0,
    RFPHY_OFF,
} RFPHY_STA;

typedef struct POWERSAVE_STA_INFO
{
    void (*txnullfn)(u8);
    void (*backupmacsettings)(void);
    void (*restoremacsettings)(void);
    void (*txexcpt)(void);
    void (*on3offexfn)(void);
    u8 associd;
    u8 ch;
    u8 ht40bw;
} POWERSAVE_STA_INFO;

typedef enum t_POWER_MODE
{
	PSACTIVE_MODE = 0,
	PSSAVING_MODE,
    TEMP_ACTIVE, //There is data to send or receive, go to active mode temporarily.
    TRANS_TO_PS, //Means there is a packet send to AP with the power management bit is power save mode.
    TRANS_TO_AC, //Means there is a packet send to AP with the power management bit is active mode.
} POWER_MODE;

void pwsave_init();
void pwsave_conf(POWERSAVE_STA_INFO *stainfo);
void checkpwstate();
void pwsave_null_report(NULL_TXRSP response);
u8 beacon_handler(u8 *pkt, u16 len, u32 intime);
u8 pwsave_set_radiotask_state(u8 state);
void pwsave_do_keepalive();
s8 activate_powersave(POWERSAVE_STA_INFO *stainfo);
s8 deactivate_powersave(u8 wificonn);
s32 switch_Power_mode(u8 enable, MCU_CFG opmode);
void on3_shutdown(u8 isprotect);
void on3_activate(u8 isprotect);

#endif /* POWERSAVE_H_ */

