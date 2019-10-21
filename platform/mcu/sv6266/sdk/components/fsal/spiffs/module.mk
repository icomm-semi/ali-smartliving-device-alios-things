
LIB_SRC := spiffs_cache.c spiffs_check.c spiffs_gc.c spiffs_hydrogen.c spiffs_nucleus.c

LIB_ASRC :=
LIBRARY_NAME := spiffs
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC :=
RELEASE_SRC := 2

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
