NAME := board_pca10056

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board pca10056
SUPPORT_MBINS      := no
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_nrf52xxx
HOST_MCU_NAME      := nrf52840

GLOBAL_INCLUDES += .

GLOBAL_DEFINES += STDIO_UART=0 CONFIG_NO_TCPIP BOARD_PCA10056 CONFIG_GPIO_AS_PINRESET FLOAT_ABI_HARD NRF52840_XXAA CONFIG_SOC_SERIES_NRF52X CONFIG_CLOCK_CONTROL_NRF5_K32SRC_250PPM=y CONFIG_BOARD_NRF52840_PCA10056=1

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_pca10056
CONFIG_SYSINFO_DEVICE_NAME := pca10056

$(NAME)_SOURCES := board.c

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_CFLAGS  += -DCONFIG_GPIO_AS_PINRESET -DFLOAT_ABI_HARD -DNRF52_PAN_74 -DCONFIG_POLL
GLOBAL_CFLAGS  += -DCONFIG_CLOCK_CONTROL_NRF5_K32SRC_XTAL
GLOBAL_CFLAGS  += -DBLE_4_2

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk

# Define default component testcase set
ifeq (, $(findstring yts, $(BUILD_STRING)))
GLOBAL_DEFINES += RHINO_CONFIG_WORKQUEUE=1
TEST_COMPONENTS += basic_test api_test wifi_hal_test rhino_test osal_test kv_test yloop_test cjson_test
else
GLOBAL_DEFINES += RHINO_CONFIG_WORKQUEUE=0
endif
