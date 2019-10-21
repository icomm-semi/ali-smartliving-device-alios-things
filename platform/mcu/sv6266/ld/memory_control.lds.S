#if defined(SETTING_THROUGHPUT_HIGH)

#else

#endif

#define M_FLASHCTRL             *sys.a:*flash.o *sys.a:*xip.o
#define M_FLASHCTRL_TEXT_RO     *sys.a:*flash.o (.text.* .rodata.*) *sys.a:*xip.o (.text.* .rodata.*)
#define M_WIFI                  *sys_arch.o *api_msg.o *edca.o *sv6266.a*:*wifi_port.o *sv6266.a*:*ethernetif.o
#define M_PERIPHERAL            *sv6266.a*:*uart.o *sv6266.a*:*libc_patch.o
#define M_OS                    *pwrmgmt.a:* *sv6266.a*:*board_cpu_pwr.o *sv6266.a*:*board_cpu_pwr_rtc.o *sv6266.a*:*board_cpu_pwr_standby_rtc.o *sv6266.a*:*board_cpu_pwr_systick.o *tmr.a:*
#define M_FLOAT                 *libgcc.a:*floatunsisf.o *libgcc.a:*divsf3.o *libgcc.a:*fixunssfsi.o *libgcc.a:*lesf2.o *libgcc.a:*mulsf3.o *libgcc.a:*fixsfsi.o *libgcc.a:*mulsf3.o *libgcc.a:*floatsisf.o *libgcc.a:*_umoddi3.o *libgcc.a:*gedf2.o *libgcc.a:*ledf2.o *libgcc.a:*eqdf2.o *libgcc.a:*muldf3.o *libgcc.a:*fixdfsi.o *libgcc.a:*floatsidf.o *libgcc.a:*adddf3.o *libgcc.a:*divdf3.o *libgcc.a:*subdf3.o *libgcc.a:*fixunsdfsi.o *libgcc.a:*floatunsidf.o *libgcc.a:*truncdfsf2.o

#define M_BOOT                  *bootloader.a:*.o *libgcc.a:*_udivdi3.o *libgcc.a:*_floatundidf.o *libgcc.a:*_div_df.o *libgcc.a:*_fixunsdfsi.o *libgcc.a:*_si_to_df.o *libc.a:*lib_a-memcpy.o
#define M_BOOT_TEXT_RO          *libgcc.a:*_udivdi3.o (.text.* .rodata.* .text ) *libgcc.a:*_floatundidf.o (.text.* .rodata.* .text ) *libgcc.a:*_div_df.o (.text.* .rodata.* .text ) *libgcc.a:*_fixunsdfsi.o (.text.* .rodata.* .text ) *libgcc.a:*_si_to_df.o (.text.* .rodata.* .text ) *libc.a:*lib_a-memcpy.o(.text.* .rodata.* .text )

#if defined(MBED_RO_TURN)
#define M_MBEDTLS               *libmbedtls.a:*aes.o *libmbedtls.a:*sha512.o *libmbedtls.a:*bignum.o *libmbedtls.a:*gcm.o
#else
#define M_MBEDTLS
#endif

#define M_FAST_BOOT             M_BOOT M_FLASHCTRL
#define M_FAST_BOOT_TEXT_RO     M_BOOT_TEXT_RO M_FLASHCTRL_TEXT_RO
#define M_ILM                   M_OS M_WIFI M_PERIPHERAL M_FLOAT
#define M_ILM_RO                M_OS M_WIFI M_PERIPHERAL M_FLOAT M_MBEDTLS

