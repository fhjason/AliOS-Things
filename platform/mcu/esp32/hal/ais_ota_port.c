#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ais_ota.h"
#include <aos/aos.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/storage.h>
#include <bluetooth/hci.h>

static flash_event_handler_t flash_handler;
static settings_event_handler_t settings_hanlder;

uint32_t ais_ota_get_setting_fw_offset()
{
    return 0;
}

void ais_ota_set_setting_fw_offset(uint32_t offset)
{

}

uint32_t ais_ota_get_page_size()
{
    return 4096;
}

static flash_event_handler_t flash_erase_hanlder;
static void flash_erase_action(void *arg)
{
     aos_msleep(50); // need do things after state changed in main task
     if (flash_erase_hanlder) flash_erase_hanlder(ALI_OTA_FLASH_ERASE_OK);
     aos_task_exit(0);
}

ali_ota_flash_err_t ais_ota_flash_erase(uint32_t const *addr, uint32_t num_pages, flash_event_handler_t cb)
{
    flash_erase_hanlder = cb;
    aos_task_new("erase_flash", flash_erase_action, NULL, 2048);
    return ALI_OTA_FLASH_CODE_SUCCESS;
}

static flash_event_handler_t flash_store_hanlder;
static void flash_store_action(void *arg)
{
     aos_msleep(20); // need do things after state changed in main task
     if (flash_store_hanlder) flash_store_hanlder(ALI_OTA_FLASH_STORE_OK);
     aos_task_exit(0);
}

ali_ota_flash_err_t ais_ota_flash_store(uint32_t const *addr, uint32_t const *p_data, uint16_t len, flash_event_handler_t cb)
{
    flash_store_hanlder = cb;
    aos_task_new("store_flash", flash_store_action, NULL, 2048);
    return ALI_OTA_FLASH_CODE_SUCCESS;
}

void ais_ota_flash_init()
{

}

void ais_ota_settings_init()
{

}

uint32_t ais_ota_get_dst_addr()
{
    return 0;
}

settings_event_handler_t setting_handler;
static void settings_action(void *arg)
{
    aos_msleep(50); // need do things after state changed in main task
    if (setting_handler) setting_handler(ALI_OTA_FLASH_STORE_OK);
    aos_task_exit(0);
}

ali_ota_settings_err_t ais_ota_settings_write(settings_event_handler_t cb)
{
    setting_handler = cb;
    aos_task_new("setting_change", settings_action, NULL, 2048);
    return ALI_OTA_SETTINGS_CODE_SUCCESS;
}

bool ais_ota_check_if_resume(uint8_t * p_data, uint16_t length)
{
    return 0;
}

void ais_ota_update_fw_version(uint8_t * p_data, uint16_t length)
{

}

bool ais_ota_check_if_update_finished()
{
    int ret;
    int len;
    char flag[5] = {0};

    len = sizeof(flag);
    ret = aos_kv_get("fwup_ongoing", flag, &len);
    if (ret < 0) {
        return false;
    }

    aos_kv_del("fwup_ongoing");

    printf("Firmware upgrade ongoing.\r\n");

    return true;
}

void ais_ota_update_settings_after_update_finished()
{

}

void ais_ota_update_setting_after_xfer_finished(uint32_t img_size, uint32_t img_crc)
{

}

#define BT_MAC_STR "bt_mac"
static ssize_t storage_write(const bt_addr_le_t *addr, u16_t key,
                             const void *data, size_t length)
{
    int ret;

    ret = aos_kv_set(BT_MAC_STR, ((bt_addr_le_t *)data)->a.val,
                     sizeof(bt_addr_le_t), 1);

    if (ret !=0 ) {
        printf("bt mac store failed.\r\n");
        return 0;
    }

    return sizeof(bt_addr_le_t);
}

static ssize_t storage_read(const bt_addr_le_t *addr, u16_t key, void *data,
                            size_t length)
{
    int ret, len = sizeof(bt_addr_le_t);

    ret = aos_kv_get(BT_MAC_STR, ((bt_addr_le_t *)data)->a.val, &len);
    if (ret != 0) {
        printf("Failed to get bt mac.\r\n");
        return 0;
    }

    return sizeof(bt_addr_le_t);
}

static int storage_clear(const bt_addr_le_t *addr)
{
    return 0;
}

int ais_ota_bt_storage_init(void)
{
    static const struct bt_storage storage = {
             .read  = storage_read,
             .write = storage_write,
             .clear = storage_clear
    };

    bt_storage_register(&storage);

    return 0;
}

int ais_ota_get_local_addr(bt_addr_le_t *addr)
{
    struct bt_le_oob oob;
    if (!addr) return -1;

    if (bt_le_oob_get_local(&oob) != 0) {
        printf("Failed to get ble local address.\r\n");
        return -1;
    }

    memcpy(addr, &(oob.addr), sizeof(oob.addr));

    return 0;
}

void ais_ota_set_upgrade_bin_type_info(ali_ota_bin_type_t type)
{

}

bool ais_ota_check_if_bins_supported()
{
    return false;
}
