LIB_SRC := 
LIB_SRC += ble_transport_api.c
LIB_SRC += bsp.c
LIB_SRC += icmd_interface_spi.c
LIB_SRC += icmd_interface_hal_spi.c
LIB_SRC += ble_main.c
LIB_SRC += terminal.c
LIB_SRC += get_ievt.c
LIB_SRC += icmd_wakelock.c
LIB_SRC += icmd2load.c
LIB_SRC += ievt_handler.c
LIB_SRC += icmd_patch_v3126_spi.c




LIB_ASRC :=
LIBRARY_NAME := condor_ble
LOCAL_CFLAGS += -Wno-address

LOCAL_AFLAGS +=

LOCAL_INC := -I$(TOPDIR)/components/inc/bsp/$(CHIP_NAME)
LOCAL_INC += -I$(TOPDIR)/components/bsp/soc
LOCAL_INC += -I$(TOPDIR)/components/bsp/soc/$(CHIP_NAME)
LOCAL_INC += -I$(TOPDIR)/components/bsp/soc/$(CHIP_NAME)/hal
LOCAL_INC += -I$(TOPDIR)/components/drv
LOCAL_INC += -I$(TOPDIR)/components/osal
LOCAL_INC += -I$(TOPDIR)/components/iotapi
LOCAL_INC += -I$(TOPDIR)/components/netstack_wrapper
LOCAL_INC += -I$(TOPDIR)/components/third_party/fdk_aac_dec_andes/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/fdk_aac_dec_andes/libAACdec/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/fdk_aac_dec_andes/libSYS/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/ble
LOCAL_INC += -I$(TOPDIR)/components/third_party/ble/include
LOCAL_INC += -I$(TOPDIR)/components/net/tcpip/lwip-1.4.0/src/include
LOCAL_INC += -I$(TOPDIR)/components/net/tcpip/lwip-1.4.0/src/include/ipv4
LOCAL_INC += -I$(TOPDIR)/components/net/tcpip/lwip-1.4.0/ports/icomm/include

RELEASE_SRC := 2

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))

