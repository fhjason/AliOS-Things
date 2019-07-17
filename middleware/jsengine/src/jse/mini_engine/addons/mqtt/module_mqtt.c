/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "be_jse_module.h"
#include "be_list.h"
#include "be_port_osal.h"
#include "be_utils.h"
/* 需要SDK支持 */
#include <mbedtls/sha1.h>

#include "core/mqtt_http.h"
#include "core/mqtt_instance.h"
#include "core/mqtt_task.h"

#include "be_jse_addon.h"
#include "board_info.h"

static const char *TAG = "MQTT";

#define CONFIG_LOGMACRO_DETAILS

#ifndef CONFIG_MQTT_TOPIC_MAX_LEN
#define CONFIG_MQTT_TOPIC_MAX_LEN 128
#endif

typedef struct {
    struct be_list_head list;
    be_jse_symbol_t *func;
    char topic_name[CONFIG_MQTT_TOPIC_MAX_LEN];
} topic_process_t;

static struct be_list_head topic_list = BE_LIST_HEAD_INIT(topic_list);

static be_jse_symbol_t *mqtt_deviceInfo(void) {
    char *productKey   = NULL;
    char *deviceName   = NULL;
    char *deviceSecret = NULL;
    be_jse_symbol_t *ret;

    be_jse_handle_function(0, NULL, NULL, NULL, NULL);

    board_getDeviceInfo(&productKey, &deviceName, &deviceSecret);

    be_jse_symbol_t *obj;
    be_jse_symbol_t *val;
    be_jse_symbol_t *child;

    obj = new_symbol(BE_SYM_OBJECT);

    if (productKey) {
        val   = new_str_symbol(productKey);
        child = add_symbol_node_name(obj, val, "productKey");
        symbol_unlock(child);
        symbol_unlock(val);
    }
    if (deviceName) {
        val   = new_str_symbol(deviceName);
        child = add_symbol_node_name(obj, val, "deviceName");
        symbol_unlock(child);
        symbol_unlock(val);
    }
    if (deviceSecret) {
        val   = new_str_symbol(deviceSecret);
        child = add_symbol_node_name(obj, val, "deviceSecret");
        symbol_unlock(child);
        symbol_unlock(val);
    }

    free(productKey);
    free(deviceName);
    free(deviceSecret);
    return obj;
}

static be_jse_symbol_t *module_handle_mqtt(be_jse_vm_ctx_t *execInfo,
                                           be_jse_symbol_t *var,
                                           const char *name) {
    int i, j;
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    MQTT_MSG_s msg;
    be_jse_symbol_t **params;
    int len;

    char productKey[16]   = {0};
    char deviceName[32]   = {0};
    char deviceSecret[48] = {0};
    char mqttDomain[64]   = {0};
    int mqttPort          = 1833;

    if (strcmp(name, "VERSION") == 0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_str_symbol("0.0.2");
    }
    if (0 == strcmp("device", name)) {
        return mqtt_deviceInfo();
    }

    if (strcmp(name, "getDeviceSecret") == 0) {
        be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);

        if (symbol_is_object(arg0)) {
            be_jse_symbol_t *productKeySymbol =
                unlock_symbol_value(lookup_named_child_symbol(
                    get_symbol_node_id(arg0), "productKey", false));
            be_jse_symbol_t *deviceNameSymbol =
                unlock_symbol_value(lookup_named_child_symbol(
                    get_symbol_node_id(arg0), "deviceName", false));
            be_jse_symbol_t *productSecretSymbol =
                unlock_symbol_value(lookup_named_child_symbol(
                    get_symbol_node_id(arg0), "productSecret", false));

            symbol_unlock(arg0);

            if (productKeySymbol == NULL || deviceNameSymbol == NULL ||
                productSecretSymbol == NULL) {
                symbol_unlock(productKeySymbol);
                symbol_unlock(deviceNameSymbol);
                symbol_unlock(productSecretSymbol);

                be_error(TAG, "参数错误");

                params = (be_jse_symbol_t **)calloc(
                    1, sizeof(be_jse_symbol_t *) * 1);
                if (params) {
                    params[0] = new_str_symbol("Invalid input parameter");
                    be_jse_post_async(arg1, params, 1);
                } else {
                    symbol_unlock(arg1);
                }

                return new_int_symbol(-1);
            }

            IOT_DEVICESECRET_s *iotDeviceSecret;
            iotDeviceSecret = calloc(1, sizeof(IOT_DEVICESECRET_s));
            if (iotDeviceSecret) {
                symbol_to_str(productKeySymbol, iotDeviceSecret->productKey,
                              sizeof(productKey) - 1);
                symbol_to_str(deviceNameSymbol, iotDeviceSecret->deviceName,
                              sizeof(deviceName) - 1);
                symbol_to_str(productSecretSymbol,
                              iotDeviceSecret->productSecret,
                              sizeof(deviceSecret) - 1);
                iotDeviceSecret->func = arg1;

                printf("[%s][%d]  productKey = %s \n", __FUNCTION__, __LINE__,
                       iotDeviceSecret->productKey);
                printf("[%s][%d]  deviceName = %s \n", __FUNCTION__, __LINE__,
                       iotDeviceSecret->deviceName);
                printf("[%s][%d]  productSecret = %s \n", __FUNCTION__,
                       __LINE__, iotDeviceSecret->productSecret);

                printf("[%s][%d]  arg1 = %p \n", __FUNCTION__, __LINE__, arg1);

                if (mqtt_http_get_instance() == NULL) {
                    mqtt_http_start(iotDeviceSecret);
                } else {
                    free(iotDeviceSecret);
                    params = (be_jse_symbol_t **)calloc(
                        1, sizeof(be_jse_symbol_t *) * 1);
                    params[0] = new_str_symbol(
                        "Resource busy, Please try again later.");

                    be_jse_post_async(arg1, params, 1);
                }
            }

            symbol_unlock(productKeySymbol);
            symbol_unlock(deviceNameSymbol);
            symbol_unlock(productSecretSymbol);
        } else {
            symbol_unlock(arg0);
            symbol_unlock(arg1);
            return new_int_symbol(-1);
        }
        return new_int_symbol(0);
    }

    if (strcmp(name, "sign") == 0) {
        be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
        if (symbol_is_object(arg0)) {
            be_jse_symbol_t *productKeySymbol =
                unlock_symbol_value(lookup_named_child_symbol(
                    get_symbol_node_id(arg0), "productKey", false));
            be_jse_symbol_t *deviceNameSymbol =
                unlock_symbol_value(lookup_named_child_symbol(
                    get_symbol_node_id(arg0), "deviceName", false));
            be_jse_symbol_t *deviceSecretSymbol =
                unlock_symbol_value(lookup_named_child_symbol(
                    get_symbol_node_id(arg0), "deviceSecret", false));
            symbol_unlock(arg0);

            if (productKeySymbol == NULL || deviceNameSymbol == NULL ||
                deviceSecretSymbol == NULL) {
                symbol_unlock(productKeySymbol);
                symbol_unlock(deviceNameSymbol);
                symbol_unlock(deviceSecretSymbol);

                be_error(TAG, "参数错误");
                return new_str_symbol(NULL);
            }

            symbol_to_str(productKeySymbol, productKey, sizeof(productKey) - 1);
            symbol_to_str(deviceNameSymbol, deviceName, sizeof(deviceName) - 1);
            symbol_to_str(deviceSecretSymbol, deviceSecret,
                          sizeof(deviceSecret) - 1);

            symbol_unlock(productKeySymbol);
            symbol_unlock(deviceNameSymbol);
            symbol_unlock(deviceSecretSymbol);

            unsigned char *content = NULL;
            content                = calloc(1, 256);
            if (content == NULL) {
                printf("[%s][%d] out of memory .\n", __FUNCTION__, __LINE__);
                return new_symbol(BE_SYM_NULL);
            }

            snprintf((char *)content, 255, "clientId%sdeviceName%sproductKey%s",
                     deviceName, deviceName, productKey);

            unsigned char k_ipad[64] = {0};
            unsigned char k_opad[64] = {0};
            unsigned char out[20]    = {0};

            memcpy(k_ipad, deviceSecret, strlen(deviceSecret));
            memcpy(k_opad, deviceSecret, strlen(deviceSecret));

            for (i = 0; i < sizeof(k_ipad); i++) {
                k_ipad[i] ^= 0x36;
                k_opad[i] ^= 0x5c;
            }

            /*use mbedtls on freertos/linux/macos*/

            mbedtls_sha1_context sha1_ctx;
            mbedtls_sha1_init(&sha1_ctx);
            mbedtls_sha1_starts(&sha1_ctx);
            mbedtls_sha1_update(&sha1_ctx, k_ipad, sizeof(k_ipad));
            mbedtls_sha1_update(&sha1_ctx, content, strlen((char *)content));
            mbedtls_sha1_finish(&sha1_ctx, out);
            mbedtls_sha1_free(&sha1_ctx);

            mbedtls_sha1_init(&sha1_ctx);
            mbedtls_sha1_starts(&sha1_ctx);
            mbedtls_sha1_update(&sha1_ctx, k_opad, sizeof(k_opad));
            mbedtls_sha1_update(&sha1_ctx, out, sizeof(out));
            mbedtls_sha1_finish(&sha1_ctx, out);
            mbedtls_sha1_free(&sha1_ctx);

            free(content);

            char sign[41];
            for (i = 0; i < sizeof(out); ++i) {
                char byte[2] = {0};
                byte[0]      = out[i] / 16;
                byte[1]      = out[i] % 16;

                for (j = 0; j < 2; ++j) {
                    if (byte[j] <= 9)
                        sign[2 * i + j] = '0' + byte[j];
                    else
                        sign[2 * i + j] = 'a' + byte[j] - 10;
                }
            }
            sign[40] = 0;

            return new_str_symbol(sign);
        }
        return new_str_symbol(NULL);
    }

    if (strcmp(name, "start") == 0) {
        be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);

        if (symbol_is_object(arg0)) {
            be_jse_symbol_t *productKeySymbol =
                unlock_symbol_value(lookup_named_child_symbol(
                    get_symbol_node_id(arg0), "productKey", false));
            be_jse_symbol_t *deviceNameSymbol =
                unlock_symbol_value(lookup_named_child_symbol(
                    get_symbol_node_id(arg0), "deviceName", false));
            be_jse_symbol_t *deviceSecretSymbol =
                unlock_symbol_value(lookup_named_child_symbol(
                    get_symbol_node_id(arg0), "deviceSecret", false));

            be_jse_symbol_t *mqttDomainSymbol =
                unlock_symbol_value(lookup_named_child_symbol(
                    get_symbol_node_id(arg0), "domain", false));
            be_jse_symbol_t *mqttPortSymbol =
                unlock_symbol_value(lookup_named_child_symbol(
                    get_symbol_node_id(arg0), "port", false));

            symbol_unlock(arg0);

            if (productKeySymbol == NULL || deviceNameSymbol == NULL ||
                deviceSecretSymbol == NULL) {
                symbol_unlock(productKeySymbol);
                symbol_unlock(deviceNameSymbol);
                symbol_unlock(deviceSecretSymbol);
                symbol_unlock(mqttDomainSymbol);
                symbol_unlock(mqttPortSymbol);

                be_error(TAG,
                         "productKey or deviceName or deviceSecret is null");
                params = (be_jse_symbol_t **)calloc(
                    1, sizeof(be_jse_symbol_t *) * 1);
                if (params) {
                    params[0] = new_str_symbol(
                        "productKey or deviceName or deviceSecret is null");
                    be_jse_post_async(arg1, params, 1);
                } else {
                    symbol_unlock(arg1);
                }

                return new_int_symbol(-1);
            }

            msg.start.cmd = MQTT_START;
            symbol_to_str(productKeySymbol, msg.start.productKey,
                          sizeof(msg.start.productKey) - 1);
            symbol_to_str(deviceNameSymbol, msg.start.deviceName,
                          sizeof(msg.start.deviceName) - 1);
            symbol_to_str(deviceSecretSymbol, msg.start.deviceSecret,
                          sizeof(msg.start.deviceSecret) - 1);
            msg.start.func = arg1;
            if (mqttDomainSymbol)
                symbol_to_str(mqttDomainSymbol, mqttDomain,
                              sizeof(mqttDomain) - 1);
            mqttPort = get_symbol_value_int(mqttPortSymbol);
            if (mqttPort == 0) mqttPort = 1883;

            symbol_unlock(productKeySymbol);
            symbol_unlock(deviceNameSymbol);
            symbol_unlock(deviceSecretSymbol);
            symbol_unlock(mqttDomainSymbol);
            symbol_unlock(mqttPortSymbol);

            /* 切换MQTT地址 */
            if (mqttDomain[0]) {
                mqtt_set_domain(mqttDomain, mqttPort);
            }

            if (mqtt_send_msg(&msg) != 0) {
                be_warn(TAG, "mqtt start failed\n");

                params = (be_jse_symbol_t **)calloc(
                    1, sizeof(be_jse_symbol_t *) * 1);
                if (params) {
                    params[0] = new_str_symbol("mqtt_send_msg error");
                    be_jse_post_async(arg1, params, 1);
                } else {
                    symbol_unlock(arg1);
                }

                return new_int_symbol(-1);
            }

            return new_int_symbol(0);
        } else {
            symbol_unlock(arg0);
            symbol_unlock(arg1);
        }
        return new_int_symbol(-1);
    }

    if (strcmp(name, "subscribe") == 0) {
        be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);

        len              = symbol_str_len(arg0);
        char *topic_name = NULL;

        if (len) {
            topic_name = calloc(1, len + 1);
            symbol_to_str(arg0, topic_name, len);
            topic_process_t *item;
            be_list_for_each_entry(item, &topic_list, list) {
                if (item == NULL) break;
                /* 判断是否重复 */
                if (strcmp(topic_name, item->topic_name) == 0) {
                    symbol_unlock(arg0);
                    symbol_unlock(arg1);
                    return new_int_symbol(-1);
                }
            }

            /* 加到list中 */
            topic_process_t *proc =
                (topic_process_t *)calloc(1, sizeof(topic_process_t));
            strncpy(proc->topic_name, topic_name, CONFIG_MQTT_TOPIC_MAX_LEN);
            proc->func = arg1;
            be_list_add_tail(&proc->list, &topic_list);

            msg.subscribe.cmd   = MQTT_SUBSCRIBE;
            msg.subscribe.topic = topic_name;
            msg.subscribe.func  = arg1;
            if (mqtt_send_msg(&msg) != 0) {
                be_list_del(&proc->list);
                free(proc);
                free(topic_name);

                symbol_unlock(arg0);
                symbol_unlock(arg1);
                return new_int_symbol(-1);
            }

            return new_int_symbol(0);

        } else {
            symbol_unlock(arg0);
            symbol_unlock(arg1);
            return new_int_symbol(-1);
        }
    }

    if (strcmp(name, "unsubscribe") == 0) {
        arg0 = be_jse_handle_single_arg_function();

        len              = symbol_str_len(arg0);
        char *topic_name = NULL;

        if (len) {
            topic_name = calloc(1, len + 1);
            symbol_to_str(arg0, topic_name, len);

            topic_process_t *item = NULL;
            be_list_for_each_entry(item, &topic_list, list) {
                if (item == NULL) break;
                if (strcmp(topic_name, item->topic_name) == 0) {
                    break;
                }
            }
            if (item) {
                /* 找到已经订阅的topic */
                msg.unsubscribe.cmd   = MQTT_UNSUBSCRIBE;
                msg.unsubscribe.topic = topic_name;
                msg.unsubscribe.func  = item->func;

                if (mqtt_send_msg(&msg) != 0) {
                    free(topic_name);
                    symbol_unlock(arg0);
                    return new_int_symbol(-1);
                }

                free(topic_name);
                be_list_del(&item->list);
                free(item);
                symbol_unlock(arg0);
                return new_int_symbol(0);
            }

            free(topic_name);
        }

        symbol_unlock(arg0);
        return new_int_symbol(-1);
    }

    if (strcmp(name, "publish") == 0) {
        be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);

        if (arg0 == NULL || arg1 == NULL || !symbol_is_string(arg0) ||
            !symbol_is_string(arg1)) {
            symbol_unlock(arg0);
            symbol_unlock(arg1);
            return new_int_symbol(-1);
        }

        int msgLen    = symbol_str_len(arg0);
        char *arg0Str = (char *)calloc(1, msgLen + 1);
        symbol_to_str(arg0, arg0Str, msgLen);

        msgLen        = symbol_str_len(arg1);
        char *arg1Str = (char *)calloc(1, msgLen + 1);
        symbol_to_str(arg1, arg1Str, msgLen);

        symbol_unlock(arg0);
        symbol_unlock(arg1);

        msg.publish.cmd     = MQTT_PUBLISH;
        msg.publish.topic   = arg0Str;
        msg.publish.payload = arg1Str;

        if (mqtt_send_msg(&msg) != 0) {
            free(arg0Str);
            free(arg1Str);
            return new_int_symbol(-1);
        }

        return new_int_symbol(0);
    }

    return BE_JSE_FUNC_UNHANDLED;
}

void module_mqtt_register(void) {
    /* 结束上一个mqtt task */
    mqtt_tsk_stop();

    /* 开启新的 mqtt task */
    mqtt_tsk_start();

    /* 注册JSE addon */
    be_jse_module_load(TAG, module_handle_mqtt);
}
