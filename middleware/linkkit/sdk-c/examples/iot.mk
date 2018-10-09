DEPENDS             := src/ref-impl/hal
DEPENDS             += src/ref-impl/tls

HDR_REFS            += src/infra
HDR_REFS            += src/services

LDFLAGS             := -Bstatic
LDFLAGS             += -liot_sdk

LDFLAGS             += -liot_hal
CFLAGS              := $(filter-out -ansi,$(CFLAGS))
LDFLAGS             += -liot_tls
ifneq (,$(filter -D_PLATFORM_IS_WINDOWS_,$(CFLAGS)))
LDFLAGS             += -lws2_32
CFLAGS              := $(filter-out -DCOAP_COMM_ENABLED,$(CFLAGS))
endif

ifneq (,$(filter -DMQTT_COMM_ENABLED,$(CFLAGS)))
TARGET                          += mqtt-example mqtt-example-rrpc mqtt-example-multithread
SRCS_mqtt-example               := app_entry.c mqtt/mqtt_example.c
SRCS_mqtt-example-rrpc          := app_entry.c mqtt/mqtt_example_rrpc.c
SRCS_mqtt-example-multithread   := app_entry.c mqtt/mqtt_example_multithread.c

    ifneq (,$(filter -DSUPPORT_ITLS,$(CFLAGS)))
    LDFLAGS += \
        -litls \
        -lid2client \
        -lkm \
        -lplat_gen \
        -lalicrypto \
        -lmbedcrypto
    endif

endif

ifneq (,$(filter -DCOAP_COMM_ENABLED,$(CFLAGS)))
TARGET              += coap-example
SRCS_coap-example   := coap/coap_example.c app_entry.c
endif

ifneq (,$(filter -DOTA_ENABLED,$(CFLAGS)))

    ifneq (,$(filter -DMQTT_COMM_ENABLED,$(CFLAGS)))
    TARGET                  += ota_mqtt-example
    SRCS_ota_mqtt-example   := ota/ota_mqtt-example.c
    endif

endif

ifneq (,$(filter -DDEVICE_MODEL_ENABLED,$(CFLAGS)))

    ifneq (,$(filter -DCONFIG_DM_DEVTYPE_SINGLE,$(CFLAGS)))
    TARGET          += linkkit-example-solo linkkit-example-countdown

    ifneq (Darwin,$(shell uname))
    TARGET          += linkkit-example-sched
    endif

    SRCS_linkkit-example-solo       := app_entry.c linkkit/linkkit_example_solo.c
    SRCS_linkkit-example-countdown  := app_entry.c linkkit/linkkit_example_cntdown.c
    SRCS_linkkit-example-sched      := app_entry.c linkkit/linkkit_example_sched.c
    endif
    
    ifneq (,$(filter -DCONFIG_DM_DEVTYPE_GATEWAY,$(CFLAGS)))
    TARGET          += linkkit-example-gw

    SRCS_linkkit-example-gw         := app_entry.c linkkit/linkkit_example_gateway.c cJSON.c
    endif
 
endif

ifneq (,$(filter -D_PLATFORM_IS_WINDOWS_,$(CFLAGS)))
    TARGET          := mqtt-example
    SRCS            := mqtt/mqtt_example.c
endif
