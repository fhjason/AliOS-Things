NAME := board_rk1108

MODULE              := 1062
HOST_ARCH           := rockchiplinux
HOST_MCU_FAMILY     := linux
SUPPORT_BINS        := no

#GLOBAL_CFLAGS += -I$(SOURCE_ROOT)/board/armhflinux/include
#GLOBAL_LDFLAGS += -L$(SOURCE_ROOT)/board/armhflinux/lib
GLOBAL_DEFINES += RHINO_CONFIG_CPU_NUM=1 RHINO_CONFIG_TASK_STACK_CUR_CHECK=1


CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_RASP
CONFIG_SYSINFO_DEVICE_NAME := RASP

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

ifneq ($(ipv6),1)
GLOBAL_DEFINES += LWIP_IPV6=0
endif
CONFIG_AOS_MESH_TAPIF := 1

CONFIG_LIB_TFS := y
CONFIG_TFS_ID2_RSA := y
CONFIG_TFS_ID2_3DES := n
CONFIG_TFS_EMULATE := n
CONFIG_TFS_ON_LINE := n
CONFIG_TFS_OPENSSL := n
CONFIG_TFS_MBEDTLS := n
CONFIG_TFS_DEBUG := n
CONFIG_TFS_VENDOR_FACTORY := csky
CONFIG_TFS_VENDOR_VERSION := tee
CONFIG_TFS_SE_LIB := libtfshal.a
CONFIG_TFS_TEE := n
CONFIG_TFS_SW := y
CONFIG_TFS_TEST := n

GLOBAL_CFLAGS += -std=gnu99
GLOBAL_INCLUDES += .
