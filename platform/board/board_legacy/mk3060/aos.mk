NAME := board_mk3060

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := configuration for board mk3060

JTAG := jlink

MODULE          := EMW3060
HOST_ARCH       := ARM968E-S
HOST_MCU_FAMILY := mcu_moc108
SUPPORT_MBINS   := no

ifeq ($(AOS_2NDBOOT_SUPPORT), yes)
$(NAME)_SOURCES := flash_partitions.c
$(NAME)_LIBSUFFIX := _2ndboot
else
$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

$(NAME)_SOURCES := board.c flash_partitions.c
endif

GLOBAL_INCLUDES += .
GLOBAL_DEFINES  += STDIO_UART=0
GLOBAL_DEFINES  += CLI_CONFIG_SUPPORT_BOARD_CMD=1

# Link Security Config
CONFIG_LS_DEBUG      := n
CONFIG_LS_ID2_OTP    := y
CONFIG_LS_KM_SE      := n
CONFIG_LS_KM_TEE     := n

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK3060
CONFIG_SYSINFO_DEVICE_NAME   := MK3060

AOS_SDK_2NDBOOT_SUPPORT := yes

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
#GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"

ifeq ($(AOS_2NDBOOT_SUPPORT), yes)
GLOBAL_LDS_INCLUDES = $($(NAME)_LOCATION)/memory$($(NAME)_LIBSUFFIX).ld.S
else
GLOBAL_LDS_INCLUDES += $($(NAME)_LOCATION)/memory.ld.S
endif

