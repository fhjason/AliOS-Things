LIBA_TARGET     := libiot_bind.a

HDR_REFS        := src/infra
HDR_REFS        += src/protocol/mqtt
HDR_REFS        += src/protocol/coap_local
HDR_REFS        += src/services/awss
HDR_REFS        += src/services/linkkit/dev_reset

ifeq (,$(filter -DALCS_ENABLED,$(CFLAGS)))
LIB_SRCS_EXCLUDE    := awss_cmp_coap.c
endif
