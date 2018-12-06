NAME := sk_gui

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Starter Kit GUI demo
$(NAME)_SOURCES := littlevgl_starterkit.c AliOS_Things_logo.c

GLOBAL_DEFINES += LITTLEVGL_STARTERKIT

$(NAME)_COMPONENTS := yloop cli

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

$(NAME)_COMPONENTS += littlevGL

GLOBAL_INCLUDES += ./
