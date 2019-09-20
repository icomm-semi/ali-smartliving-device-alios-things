NAME := http
$(NAME)_SOURCES := app_entry.c \
                   http-example.c

GLOBAL_DEFINES +=  MBEDTLS_SSL_MAX_CONTENT_LEN=10240

$(NAME)_COMPONENTS += framework/protocol/linkkit/sdk \
                      framework/protocol/linkkit/hal \
                      framework/netmgr \
                      framework/common \
                      utility/cjson \
                      tools/cli

GLOBAL_DEFINES += CONFIG_AOS_CLI COAP_DTLS_SUPPORT
GLOBAL_DEFINES += DISABLE_SECURE_STORAGE

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif
