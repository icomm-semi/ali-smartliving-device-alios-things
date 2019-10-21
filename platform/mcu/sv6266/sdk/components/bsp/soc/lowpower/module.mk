
LIB_SRC := rtos_lowpower.c

LIB_ASRC :=
LIBRARY_NAME := lowpower
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=
LOCAL_INC := -I$(TOPDIR)/components/inc/bsp/$(CHIP_NAME)
LOCAL_INC += -I$(TOPDIR)/components/drv
LOCAL_INC += -I$(TOPDIR)/components/drv/gpio

RELEASE_SRC := 1

$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
