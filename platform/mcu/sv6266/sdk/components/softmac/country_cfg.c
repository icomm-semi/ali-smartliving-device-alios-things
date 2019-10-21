#include "stdio.h"
#include "wificonf.h"
#include "country_cfg.h"
#include "tiramisu/countryinfo.h"

CHANNEL_INFO channel_tbl[] = {  {36, 0x1, 0}, 
                                {40, 0x3, 0}, 
                                {44, 0x1, 0}, 
                                {48, 0x3, 0}, 
                                {52, 0x1, 0}, 
                                {56, 0x3, 0}, 
                                {60, 0x1, 0}, 
                                {64, 0x3, 0}, 
                                {100, 0x1, 0}, 
                                {104, 0x3, 0}, 
                                {108, 0x1, 0}, 
                                {112, 0x3, 0}, 
                                {116, 0x1, 0}, 
                                {120, 0x3, 0}, 
                                {124, 0x1, 0}, 
                                {128, 0x3, 0}, 
                                {132, 0x1, 0}, 
                                {136, 0x3, 0}, 
                                {140, 0x1, 0}, 
                                {149, 0x1, 0}, 
                                {153, 0x3, 0}, 
                                {157, 0x1, 0}, 
                                {161, 0x3, 0}, 
                                {165, 0, 0}};

extern IEEE80211STATUS gwifistatus;

s32 check_2Gchannel_availavble(u8 ch)
{
    s32 ret = -1;

    if(ch > 14)
        goto exit;
    
    if(gwifistatus.customconf.available_2Gchannel & (0x1 << (ch - 1)))
        ret = 1;

exit:
    return ret;
}
extern is_5g_band_support(void);
s32 check_5Gchannel_availavble(u8 ch, u8 *dfs)
{
    FREQ_INFO *tmpfreq;
    u16 lowerbound, upperbound;
    int i, j;
    s32 ret = -1;
    
    if(FALSE== is_5g_band_support() || ch <= 14)
        return ret;
    
    for(i = 0; i < sizeof(channel_tbl)/sizeof(CHANNEL_INFO); i++)
    {
        if(channel_tbl[i].ch == ch)
        {
            lowerbound = 5000 + ch * 5 - 10;
            upperbound = 5000 + ch * 5 + 10;
            for(j = 0; j < MAX_REG_RULES; j++)
            {
                tmpfreq = &gcoubtryinfo[gwifistatus.countrycode].freq[j];
                if((tmpfreq->start_freq_mhz <= lowerbound) && (upperbound <= tmpfreq->end_freq_mhz))
                {
                    ret = channel_tbl[i].ht40secch;
                    *dfs = channel_tbl[i].dfs;
                    break;
                }
            }
            break;
        }
    }

    return ret;
}

s32 check_channel_availavble(u8 ch)
{
    u8 dfs;
    if(ch <= 14)
        return check_2Gchannel_availavble(ch);
    else
        return check_5Gchannel_availavble(ch, &dfs);
}

u8 find_5G_chnindex(u8 ch)
{
    u8 i;
    u8 index = 0;
    
    for(i = 0; i < sizeof(channel_tbl)/sizeof(CHANNEL_INFO); i++)
    {
        if(channel_tbl[i].ch == ch)
        {
            index = i;
            break;
        }

    }
    if(i == sizeof(channel_tbl)/sizeof(CHANNEL_INFO))
    {
        printf("wierd ch:%d\n", ch);
    }

    return index;
}

/*u8 find_5G_firstch(u8 freq)
{
    u8 i;
    u8 ch = 0;
    
    for(i = 0; i < sizeof(wifi_5g_channel_tbl); i++)
    {
        ch = wifi_5g_channel_tbl[i];

    }

    return index;
}*/

void set_5G_availch(u32 start, u32 end, u32 flag)
{
    u8 ch, index;
    u32 frequpper;
    
    ch = (start + 10  - WIFI_5G_BASEFREQ)/5 + 1;
    while(1)
    {
        frequpper = 5 * (ch - 1) + WIFI_5G_BASEFREQ + 10;
        if(frequpper <= end)
        {
            index = find_5G_chnindex(ch);
            if(flag)
                channel_tbl[index].dfs = 1;
            gwifistatus.customconf.available_5Gchannel |= 0x1 << index;
            ch += 4;
        }
        else
        {
            break;
        }
    }
}

void set_2G_availch(u32 start, u32 end)
{
    u8 ch;
    u32 frequpper;
    
    ch = (start + 10 - WIFI_2G_BASEFREQ)/5 + 1;

    while (1)
    {
        frequpper = 5 * (ch - 1) + WIFI_2G_BASEFREQ + 10;
        if(frequpper <= end)
        {
            gwifistatus.customconf.available_2Gchannel |= 0x1 << (ch - 1);
            ch++;
        }
        else
        {
            break;
        }
    }

}

int config_channel(COUNTRY_CODE index)
{
    int i;
    int countrymax = sizeof(gcoubtryinfo)/sizeof(COUNTRY_INFO);
    
    if (index >= countrymax)
        return -1;

    gwifistatus.countrycode = index;
    gwifistatus.customconf.available_2Gchannel = 0;
    gwifistatus.customconf.available_5Gchannel = 0;

    for (i = 0; i < MAX_REG_RULES; i++)
    {
        if (gcoubtryinfo[index].freq[i].start_freq_mhz == 0)
            break;

//        printf("bank:%x %d\n", i, gcoubtryinfo[index].freq[i].start_freq_mhz);
        if (gcoubtryinfo[index].freq[i].start_freq_mhz >= 5000)
            set_5G_availch(gcoubtryinfo[index].freq[i].start_freq_mhz, gcoubtryinfo[index].freq[i].end_freq_mhz, gcoubtryinfo[index].freq[i].flags);
        else
            set_2G_availch(gcoubtryinfo[index].freq[i].start_freq_mhz, gcoubtryinfo[index].freq[i].end_freq_mhz);
    }

//    printf("2G:%x\n", gwifistatus.customconf.available_2Gchannel);
//    printf("5G:%x\n", gwifistatus.customconf.available_5Gchannel);
    return 0;
}

