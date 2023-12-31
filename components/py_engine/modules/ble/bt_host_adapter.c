/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "ulog/ulog.h"

#include "modble.h"

#define LOG_TAG "BT_HOST_ADAPTER"

static void bt_host_adapter_adv_data_destroy(ad_data_t *ad_data, int ad_num);
static int bt_host_adapter_event_callback(ble_event_en event, void *event_data);
extern int bt_gatts_adapter_event_callback(ble_event_en event,
                                           void *event_data);
#ifdef CHIP_HAAS1000
extern int netdev_set_epta_params(int wlan_duration, int bt_duration,
                                  int hw_epta_enable);
#endif

static ble_event_cb_t bt_host_adapter_ble_cb = {
    .callback = bt_host_adapter_event_callback,
};

static int bt_host_adapter_char2hex(char c, char *x)
{
    if (c >= '0' && c <= '9') {
        *x = c - '0';
    } else if (c >= 'a' && c <= 'f') {
        *x = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        *x = c - 'A' + 10;
    } else {
        return -1;
    }

    return 0;
}

static int bt_host_adapter_hex2bin(char *hex, uint8_t *buf)
{
    char dec;
    int hexlen, i;

    hexlen = strlen(hex);
    if (hexlen == 0) {
        return 0;
    }

    /* if hexlen is uneven, return failed */
    if (hexlen % 2) {
        return 0;
    }

    /* regular hex conversion */
    for (i = 0; i < hexlen / 2; i++) {
        if (bt_host_adapter_char2hex(hex[2 * i], &dec) < 0) {
            return 0;
        }
        buf[i] = dec << 4;

        if (bt_host_adapter_char2hex(hex[2 * i + 1], &dec) < 0) {
            return 0;
        }
        buf[i] += dec;
    }

    return hexlen / 2;
}

int bt_host_adapter_bin2hex(char *hex, uint8_t *buf, uint8_t buf_len)
{
    int i;

    if (hex == NULL)
        return 0;

    hex[0] = 0;
    for (i = 0; i < buf_len; i++) {
        sprintf(hex + strlen(hex), "%02x", buf[i]);
    }

    return buf_len * 2;
}

static int bt_host_adapter_addr2hex(char *hex, uint8_t *addr)
{
    int i;

    if (hex == NULL)
        return 0;

    hex[0] = 0;
    for (i = 0; i < 6; i++) {
        sprintf(hex + strlen(hex), "%02x:", addr[5 - i]);
    }
    hex[17] = 0;

    return i * 3 - 1;
}

static int bt_host_adapter_event_callback(ble_event_en event, void *event_data)
{
    LOGD(LOG_TAG, "%s, event = %x", __func__, event);
    switch (event) {
    case EVENT_GAP_CONN_CHANGE:
        {
            evt_data_gap_conn_change_t *e =
                (evt_data_gap_conn_change_t *)event_data;
            int32_t connect;

            if (e->connected == CONNECTED) {
                connect = 1;
#ifdef CHIP_HAAS1000
                netdev_set_epta_params(40000, 60000, 0);
#endif
            } else {
                connect = 0;
            }

            py_ble_event_notify(e->conn_handle, connect);
        }
        break;

    default:
        bt_gatts_adapter_event_callback(event, event_data);
        break;
    }

    return 0;
}

static ad_data_t *bt_host_adapter_adv_data_parse(char *adv_data, int8_t *num)
{
    int ad_num;
    int i, type, len, adv_len;
    ad_data_t *ad_data;
    uint8_t adv_bin[31];

    if (adv_data == NULL) {
        return NULL;
    }
    adv_len = strlen(adv_data);
    if (adv_len == 0 || adv_len > 62) {
        return NULL;
    }

    adv_len = bt_host_adapter_hex2bin(adv_data, adv_bin);

    ad_num = 0;
    for (i = 0; i < adv_len;) {
        len = adv_bin[i];
        if (i + len >= adv_len) {
            LOGD(LOG_TAG, "%s, wrong data %s", __func__, adv_data);
            return NULL;
        }

        i += len + 1;
        ad_num++;
    }
    if (ad_num == 0) {
        return NULL;
    }

    ad_data = aos_malloc(ad_num * sizeof(ad_data_t));
    if (ad_data == NULL) {
        return NULL;
    }
    memset(ad_data, 0, sizeof(ad_num * sizeof(ad_data_t)));
    ad_num = 0;
    for (i = 0; i < adv_len;) {
        len = adv_bin[i];
        ad_data[ad_num].len = len - 1;
        ad_data[ad_num].type = adv_bin[i + 1];
        ad_data[ad_num].data = aos_malloc(len - 1);
        if (ad_data[ad_num].data == NULL) {
            bt_host_adapter_adv_data_destroy(ad_data, ad_num);
            return NULL;
        }
        memcpy(ad_data[ad_num].data, adv_bin + i + 2, len - 1);

        i += len + 1;
        ad_num++;
    }

    if (num) {
        *num = ad_num;
    }
    return ad_data;
}

static void bt_host_adapter_adv_data_destroy(ad_data_t *ad_data, int ad_num)
{
    int i;

    for (i = 0; i < ad_num; i++) {
        if (ad_data[i].data) {
            aos_free(ad_data[i].data);
        }
    }
    aos_free(ad_data);
}

int py_bt_host_adapter_init(amp_bt_host_adapter_init_t *init)
{
    init_param_t param = { 0 };
    int ret = -1;

    param.dev_name = init->dev_name;
    param.conn_num_max = init->conn_num_max;
    param.dev_addr = NULL;
    ret = ble_stack_init(&param);
    if (ret != BLE_STACK_OK) {
        return -1;
    }

#ifdef CHIP_HAAS1000
    /* HaaS100/EDU WI-FI/蓝牙共存设置 */
    netdev_set_epta_params(40000, 60000, 0);
#endif

    ret = ble_stack_event_register(&bt_host_adapter_ble_cb);
    if (ret) {
        return -1;
    }

    return ret;
}

int bt_host_adapter_start_adv(amp_bt_host_adapter_adv_start_t *adv_param)
{
    adv_param_t param;
    int ret;

    param.type = adv_param->type;
    param.interval_min = adv_param->interval_min;
    param.interval_max = adv_param->interval_max;
    param.channel_map = adv_param->channel_map;

    param.ad_num = 0;
    param.sd_num = 0;
    param.ad =
        bt_host_adapter_adv_data_parse(adv_param->adv_data, &(param.ad_num));
    param.sd = bt_host_adapter_adv_data_parse(adv_param->scan_rsp_data,
                                              &(param.sd_num));

    param.filter_policy = ADV_FILTER_POLICY_ANY_REQ;
    memset(&(param.direct_peer_addr), 0, sizeof(dev_addr_t));
    LOGD(LOG_TAG,
         "%s, ble_stack_adv_start, type = %d, min = %d, max = %d, ch = %d, "
         "ad_num = %d, sd_num = %d, ad[0].type = %d, ad[0].len = %d",
         __func__, param.type, param.interval_min, param.interval_max,
         param.channel_map, param.ad_num, param.sd_num, param.ad[0].type,
         param.ad[0].len);
    ret = ble_stack_adv_start(&param);
    LOGD(LOG_TAG, "ble_stack_adv_start ret = %d", ret);
    if (param.ad) {
        bt_host_adapter_adv_data_destroy(param.ad, param.ad_num);
    }
    if (param.sd) {
        bt_host_adapter_adv_data_destroy(param.sd, param.sd_num);
    }
    if (ret) {
        return -1;
    }

    return ret;
}

int bt_host_adapter_stop_adv(void)
{
    int ret;

    ret = ble_stack_adv_stop();
    if (ret) {
        return -1;
    }

    return ret;
}

int bt_host_adapter_start_scan(amp_bt_host_adapter_scan_start_t *scan_param)
{
    scan_param_t param;
    int ret;

    param.type = scan_param->type;
    param.interval = scan_param->interval;
    param.window = scan_param->window;
    param.filter_dup = SCAN_FILTER_DUP_DISABLE;
    param.scan_filter = SCAN_FILTER_POLICY_ANY_ADV;
    LOGD(LOG_TAG, "%s, ble_stack_scan_start, type = %d, int = %d, win = %d",
         __func__, param.type, param.interval, param.window);
    ret = ble_stack_scan_start(&param);
    LOGD(LOG_TAG, "ble_stack_scan_start ret = %d", ret);
    if (ret) {
        return -1;
    }

    return ret;
}

int bt_host_adapter_stop_scan(void)
{
    int ret;

    ret = ble_stack_scan_stop();
    if (ret) {
        return -1;
    }

    return ret;
}
