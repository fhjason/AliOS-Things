NAME := netmgr

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES-y := netmgr.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS-y      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS-y      += -Wall -Werror
endif

net ?=  wifi
ifeq ($(net), wifi)
ifneq (,$(ssid))
$(NAME)_DEFINES-y += WIFI_SSID=\"$(ssid)\"
$(NAME)_DEFINES-y += WIFI_PWD=\"$(pwd)\"
endif
$(NAME)_SOURCES-y += interfaces/netmgr_wifi.c
GLOBAL_DEFINES-y += NET_WITH_WIFI
$(NAME)_COMPONENTS-y += halwifi
else
$(NAME)_SOURCES-y += hal/net.c
$(NAME)_SOURCES-y += interfaces/netmgr_net.c
GLOBAL_INCLUDES-y += ../include/hal/
endif

$(NAME)_COMPONENTS-y += kernel.fs.kv yloop kernel.hal

GLOBAL_INCLUDES-y += include ../../middleware/alink/protocol/os/platform/

GLOBAL_DEFINES-y += AOS_NETMGR
