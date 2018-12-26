NAME := sal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := socket adapter layer

GLOBAL_DEFINES += WITH_SAL                   \
                  SAL_USE_AOS_HAL

$(NAME)_SOURCES := ./src/sal_sockets.c \
                   ./src/sal_err.c     \
                   ./src/ip4_addr.c    \
                   ./src/sal_arch_aos.c

GLOBAL_INCLUDES += ./                  \
                   ./include

ifeq (wifi.gt202,$(module))
$(NAME)_COMPONENTS += drivers.sal.wifi.gt202
else ifeq (wifi.mk3060,$(module))
$(NAME)_COMPONENTS += drivers.sal.wifi.mk3060
else ifeq (wifi.bk7231,$(module))
$(NAME)_COMPONENTS += drivers.sal.wifi.bk7231
else ifeq (gprs.sim800,$(module))
$(NAME)_COMPONENTS += drivers.sal.gprs.sim800
else ifeq (wifi.esp8266,$(module))
$(NAME)_COMPONENTS += drivers.sal.wifi.esp8266
else ifeq (wifi.athost,$(module))
$(NAME)_COMPONENTS += drivers.sal.wifi.athost
endif
