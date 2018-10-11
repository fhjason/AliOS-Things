NAME := board_cb2201

JTAG := jlink

$(NAME)_TYPE := kernel
MODULE               := CH2201
HOST_CHIP            := ch2201
HOST_ARCH            := ck802
HOST_MCU_FAMILY      := csky
SUPPORT_BINS         := no

#$(NAME)_COMPONENTS  :=  tfs
#CONFIG_LIB_TFS := y
#CONFIG_TFS_ID2_RSA := y
#CONFIG_TFS_ID2_3DES := n
#CONFIG_TFS_EMULATE := n
#CONFIG_TFS_ON_LINE := n
#CONFIG_TFS_OPENSSL := n
#CONFIG_TFS_MBEDTLS := n
#CONFIG_TFS_ALICRYPTO := n
#CONFIG_TFS_DEBUG := n
#CONFIG_TFS_TEE := n
#CONFIG_TFS_SW := y
#CONFIG_TFS_TEST := n

$(NAME)_SOURCES := board_init.c

ifeq ($(LWIP),1)
$(NAME)_SOURCES += net/ethernetif.c
endif

GLOBAL_INCLUDES += include/
GLOBAL_DEFINES += STDIO_UART=0 MBEDTLS_AES_ROM_TABLES=1


CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_CKHOBBIT
CONFIG_SYSINFO_DEVICE_NAME := CKHOBBIT

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -std=gnu99
GLOBAL_LDFLAGS  += -Wl,-ckmap='cb2201.map' -Wl,-zmax-page-size=1024 -lm

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk
