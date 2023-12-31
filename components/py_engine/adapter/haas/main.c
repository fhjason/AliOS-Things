/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <aos/errno.h>
#include <aos/kernel.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "amp_task.h"
#include "aos/init.h"
#include "aos/vfs.h"
#include "aos_hal_uart.h"
#include "board.h"
#include "board_mgr.h"
#include "cJSON.h"
#include "mpstdinport.h"
#include "netmgr.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/repl.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py_defines.h"
#include "shared/runtime/pyexec.h"
#include "sys/stat.h"
#include "ulog/ulog.h"

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "haas_main.h"
#define LOG_TAG "haas_main"

static int8_t *stack_top = NULL;
static bool isPythonRunning = false;
static uint32_t stack_size = 1024;

#if MICROPY_ENABLE_GC
static int8_t *heap = NULL;
#ifndef MICROPY_GC_HEAP_SIZE
#define MICROPY_GC_HEAP_SIZE (1024 * 64)
#endif
#endif

static mpy_thread_args *alloc_mpy_thread_args(int32_t argc, int8_t **argv)
{
    mpy_thread_args *temp_args = (mpy_thread_args *)calloc(1, sizeof(mpy_thread_args));
    if (temp_args == NULL) {
        LOGE(LOG_TAG, "%s;malloc mpy_thread_args failed\n", __func__);
        return NULL;
    }

    if (argc == 1) {
        temp_args->is_repl_mode = true;
    } else {
        temp_args->is_repl_mode = false;
    }
    temp_args->argc = argc;

    temp_args->argv = (int8_t **)calloc(1, sizeof(int8_t *) * temp_args->argc);
    if (temp_args->argv == NULL) {
        LOGE(LOG_TAG, "%s:temp_args->argv alloc memory failed\n", __func__);
        free(temp_args);
        return NULL;
    }

    for (int32_t i = 0; i < argc; i++) {
        temp_args->argv[i] = strdup(argv[i]);
    }

    return temp_args;
}

static int32_t free_mpy_thread_args(mpy_thread_args *args)
{
    if (args == NULL) {
        LOGE(LOG_TAG, "args is illegal\n");
        return -1;
    }

    for (int32_t i = 0; i < args->argc; i++) {
        if (args->argv[i] != NULL) {
            free(args->argv[i]);
        }
    }

    free(args->argv);
    free(args);
    return 0;
}

static bool is_file_exist(const int8_t *path)
{
    if (path == NULL) {
        LOGE(LOG_TAG, "File path null\n");
        return false;
    }

    FILE *fd = fopen(path, "r");
    if (fd != NULL) {
        fclose(fd);
        return true;
    }

    return false;
}

static uint8_t *is_mainpy_exist()
{
    /* check whether main/pyamp/main.py */
    FILE *fd = fopen(AMP_PY_ENTRY_DEFAULE, "r");
    if (fd != NULL) {
        fclose(fd);
        return AMP_PY_ENTRY_DEFAULE;
    }

    fd = fopen(AMP_PY_ENTRY_BAK, "r");
    if (fd != NULL) {
        fclose(fd);
        return AMP_PY_ENTRY_BAK;
    }

    fd = fopen(MP_PY_ENTRY_BAK, "r");
    if (fd != NULL) {
        fclose(fd);
        return MP_PY_ENTRY_BAK;
    }

    return NULL;
}

static void net_init()
{
#define WIFI_DEV_PATH "/dev/wifi0"

    int8_t *wificonf = MP_FS_ROOT_DIR "/wifi.conf";
    if (is_file_exist(wificonf) == true) {
        event_service_init(NULL);
        netmgr_service_init(NULL);
        netmgr_set_auto_reconnect(0, true);
        netmgr_wifi_set_auto_save_ap(true);

        netmgr_add_dev(WIFI_DEV_PATH);
        netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);
    } else {
        // need init event service because of eth net
        event_service_init(NULL);
        netmgr_service_init(NULL);
        netmgr_set_auto_reconnect(0, true);
        netmgr_wifi_set_auto_save_ap(true);
    }
}

#include "extmod/vfs.h"
#include "extmod/vfs_posix.h"
// Try to mount the data on "/data" and chdir to it for the boot-up directory.
static int32_t mount_fs(int8_t *mount_point_str)
{
    mp_obj_t mount_point = mp_obj_new_str(mount_point_str, strlen(mount_point_str));
    mp_obj_t bdev = mp_type_vfs_posix.make_new(&mp_type_vfs_posix, 0, 0, NULL);
    int32_t ret = mp_vfs_mount_and_chdir_protected(bdev, mount_point);
    if (ret != 0) {
        printf("mount_fs failed with mount_point: %s\n", mount_point_str);
        return -MP_ENOENT;
    }
    return 0;
}

static void py_engine_mainloop(void *p)
{
    mpy_thread_args *args = (mpy_thread_args *)p;
    if (args == NULL) {
        LOGE(LOG_TAG, "%s:args is illegal\n", __func__);
        return;
    }

    int32_t ret = mpy_init(args);
    if (ret) {
        LOGE(LOG_TAG, "%s:mpy_init failed, ret=%d\n", __func__, ret);
    } else {
        /* Suspend CLI to make sure REPL use UART exclusively */
        aos_cli_suspend();

        isPythonRunning = true;
        mpy_run(args->argc, args->argv);

        /* Resume CLI after REPL */
        aos_cli_resume();
    }

    free_mpy_thread_args(args);
    mpy_deinit();
    isPythonRunning = false;
}

static void py_engine_task(void *p)
{
    py_task_init();
    while (1) {
        /* loop for asynchronous operation */
        if (py_task_yield(200) == 1) {
            printf("pyengine task yield exit! \r\n");
            break;
        }
    }
    aos_task_exit(0);
}

// #if PY_CHANNEL_ENABLE
// static void network_func(void *argv)
// {
//     while (!aos_get_network_status()) {
//         aos_msleep(1000);
//     }
//     //py_app_management_center_init();
//     extern int pyamp_app_upgrade(char *url);
//     amp_otaput_init(pyamp_app_upgrade);
//     aos_task_exit(0);
//     return;
// }
// #endif

static void python_entry(int32_t argc, int8_t **argv)
{
    if (isPythonRunning == true) {
        printf(" **************************************************** \r\n");
        printf(" ** Python is running, cannot start another engine ** \r\n");
        printf(" **************************************************** \r\n");

        return;
    }

    printf(" Welcome to MicroPython \n");

    aos_task_t engine_entry = NULL;
    mpy_thread_args *args = alloc_mpy_thread_args(argc, argv);

    int32_t ret =
        aos_task_new_ext(&engine_entry, "py_engine", py_engine_mainloop, args, 1024 * 20, AOS_DEFAULT_APP_PRI);
    if (ret != 0) {
        LOGE(LOG_TAG, "py_engine_mainloop task creat failed!");
        return;
    }
}

char *board_get_json_buff(const char *json_path);

static aos_log_level_t get_logLevel()
{
    int32_t ret = -1;
    int8_t logLevelStr[16] = { 0 };
    int8_t *data_root_path = AMP_FS_ROOT_DIR "/board.json";
    int8_t *board_json_path = MP_FS_ROOT_DIR "/python-apps/driver/board.json";

    FILE *json_fd = fopen(data_root_path, "r");
    if (json_fd != NULL) {
        fclose(json_fd);
        board_json_path = data_root_path;
    }

    int8_t *json_buff = board_get_json_buff(board_json_path);
    if (NULL == json_buff) {
        goto debugLevel_kv; /* get debugLevel from kv */
    }

    cJSON *root = cJSON_Parse(json_buff);
    if (NULL == root) {
        goto debugLevel_kv; /* get debugLevel from kv */
    }

    cJSON *debug = cJSON_GetObjectItem(root, APP_CONFIG_DEBUG);
    if (debug != NULL) {
        if (!cJSON_IsString(debug)) {
            goto debugLevel_kv;
        } else {
            strcpy(logLevelStr, debug->valuestring);
            goto debugLevel_parser;
        }
    }

debugLevel_kv:
    if (json_buff != NULL) {
        aos_free(json_buff);
    }

    int32_t len = sizeof(logLevelStr);
    ret = aos_kv_get(APP_CONFIG_DEBUG, logLevelStr, &len);
    if (ret != 0) {
        uint8_t *set_value = "ERROR";
        aos_kv_set(APP_CONFIG_DEBUG, set_value, strlen(set_value), 1);
        strcpy(logLevelStr, set_value);
    }

debugLevel_parser:
    if (strcmp(logLevelStr, "DEBUG") == 0) {
        return AOS_LL_DEBUG;
    } else if (strcmp(logLevelStr, "INFO") == 0) {
        return AOS_LL_INFO;
    } else if (strcmp(logLevelStr, "WARN") == 0) {
        return AOS_LL_WARN;
    } else if (strcmp(logLevelStr, "ERROR") == 0) {
        return AOS_LL_ERROR;
    } else if (strcmp(logLevelStr, "FATAL") == 0) {
        return AOS_LL_FATAL;
    } else if (strcmp(logLevelStr, "NONE") == 0) {
        return AOS_LL_NONE;
    }
    return AOS_LL_ERROR;
}

void haas_main(int32_t argc, int8_t **argv)
{
    int32_t ret = -1;

#if PY_BOOT_ENABLE
    aos_cli_suspend();
    pyamp_boot_main();
    aos_cli_resume();
#endif

#if PY_BUILD_KV
    /* kv module init */
    ret = kv_init();
    if (ret != 0) {
        LOGE(LOG_TAG, "kv init failed!");
        return;
    }
#endif

    /* ulog module init */
    ulog_init();
    aos_log_level_t log_level = get_logLevel();
    aos_set_log_level(log_level);
    /* net init */
    // net_init();

    aos_task_t engine_task;
    ret = aos_task_new_ext(&engine_task, "py_engine_task", py_engine_task, NULL, 1024 * 8, AOS_DEFAULT_APP_PRI);
    if (ret != 0) {
        LOGE(LOG_TAG, "pyengine task creat failed!");
        return;
    }

// #if PY_CHANNEL_ENABLE
//     aos_task_t network_task;
//     /* network start */
//     ret = aos_task_new_ext(&network_task, "mpy_network", network_func, NULL, 1024 * 8, AOS_DEFAULT_APP_PRI);
//     if (ret != 0) {
//         LOGE(LOG_TAG, "network task creat failed!");
//         return ret;
//     }
// #endif

    /* Check whether we have main.py to execute */
    // defalut entry python engine
    // uint8_t *path = is_mainpy_exist();
    // if (path != NULL) {
    //     uint8_t *argv[2] = { "python", path };
    //     python_entry(2, &argv);
    // }
        uint8_t *param[1] = { "python" };
        python_entry(1, &param);
}

static void handle_unzip_cmd(int32_t argc, int8_t **argv)
{
    int8_t *zippath;
    int8_t *destpath;
    int32_t ret = 0;
    if (argc < 2) {
        LOGE(LOG_TAG, "Error params,Usage: unzip /data/src.zip  /sdcard \r\n");
        return;
    }

    if (argc == 2) {
        destpath = "/data";
    } else {
        destpath = argv[2];
    }
    zippath = argv[1];
    ret = miniUnzip(zippath, destpath);
    if (ret) {
        LOGE(LOG_TAG, "unzip failed ,errno is %d \r\n", ret);
    } else {
        LOGD(LOG_TAG, "unzip succeed \r\n");
    }
}

#if MICROPY_ENABLE_COMPILER
void do_str(const int8_t *src, mp_parse_input_kind_t input_kind)
{
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

int32_t mpy_init(mpy_thread_args *args)
{
    int32_t ret = -1;

    mp_stdin_init(MP_REPL_UART_PORT, MP_REPL_UART_BAUDRATE);

#if MICROPY_PY_THREAD
    void *stack_addr = mp_sal_get_stack_addr();
    stack_size = mp_sal_get_stack_size();
    mp_thread_init(stack_addr, stack_size);
#endif

#if MICROPY_ENABLE_GC
    heap = (int8_t *)malloc(MICROPY_GC_HEAP_SIZE);
    if (NULL == heap) {
        LOGE(LOG_TAG, "mpy_init: heap alloc fail!\r\n");
        return -1;
    }
#endif

    return 0;
}

int32_t mpy_deinit(void)
{
    // need relese python run mem
#if MICROPY_ENABLE_GC
    if (NULL != heap) {
        free(heap);
        heap == NULL;
    }
#endif

    mp_stdin_deinit();
}

static void set_sys_argv(int8_t *argv[], int32_t argc, int32_t start_arg)
{
    for (int32_t i = start_arg; i < argc; i++) {
        mp_obj_list_append(mp_sys_argv, MP_OBJ_NEW_QSTR(qstr_from_str(argv[i])));
    }
}

int32_t is_download_mode(uint32_t wait_time_ms)
{
    int32_t ret = 0;
    int32_t c = 0;
    int64_t now_time = 0;
    int64_t begin_time = aos_now_ms();

    do {
        int32_t c = ringbuf_get(&stdin_ringbuf);
        if (c == '\x03') {
            ret = 1;
            break;
        }
        now_time = aos_now_ms();
        uint32_t time = now_time - begin_time;
        if (time >= wait_time_ms) {
            ret = 0;
            break;
        }
        pyamp_boot_delay(1);
    } while (1);

    return ret;
}

int32_t mpy_run(int32_t argc, int8_t *argv[])
{
    int32_t ret = 0;
    int32_t mode = 0;

soft_reset:

    mp_stack_set_top(stack_top);
    // adjust the stack_size to provide room to recover from hitting the
    // limit
#if MICROPY_STACK_CHECK
    mp_stack_ctrl_init();
    mp_stack_set_limit(stack_size * sizeof(cpu_stack_t) - 1024);
#endif
    // reinit gc
#if MICROPY_ENABLE_GC
    gc_init(heap, heap + MICROPY_GC_HEAP_SIZE);
#endif

    mp_init();

#if MICROPY_VFS_POSIX
    {
        // Mount the host FS at the root of our internal VFS
        ret = mount_fs("/");
        if (ret != 0) {
            printf(" !!!!!!!! %s, %d, faild to mount fs !!!!!!!!\n", __func__, __LINE__);
        }
        MP_STATE_VM(vfs_cur) = MP_STATE_VM(vfs_mount_table);
    }
#endif

    /*set default mp_sys_path*/
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path,
                       MP_OBJ_NEW_QSTR(MP_QSTR_));  // current dir (or base dir of the script)
    // add /data/pyamp to system path
    int8_t *path = MP_FS_ROOT_DIR "/pyamp";
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr(path, strlen(path)));

    // add /data/lib to system path
    path = MP_FS_ROOT_DIR "/lib";
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr(path, strlen(path)));

    // add /sdcard/pyamp to system path
    path = MP_FS_EXT_ROOT_DIR "/pyamp";
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr(path, strlen(path)));

    // add /sdcard/lib to system path
    path = MP_FS_EXT_ROOT_DIR "/lib";
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr(path, strlen(path)));

    mp_obj_list_init(mp_sys_argv, 0);

// #if 0
    // mode = is_download_mode(300); /* Only available on HaaS506 */
// #endif

    // if (argc >= 2 && mode == 0) {
    //     int8_t *filename = argv[1];
    //     int8_t filepath[256] = { 0 };

    //     if (filename[0] != '/') {
    //         getcwd(filepath, sizeof(filepath));
    //         strcat(filepath, "/");
    //         strcat(filepath, filename);
    //     } else {
    //         strcpy(filepath, filename);
    //     }

    //     int8_t *p = strrchr(filepath, '/');
    //     size_t len = 0;
    //     mp_obj_t *path_items = NULL;
    //     mp_obj_list_get(mp_sys_path, &len, &path_items);

    //     if (p >= filepath) {
    //         path_items[0] = mp_obj_new_str_via_qstr(filepath, p - filepath);
    //     } else {
    //         path_items[0] = mp_obj_new_str_via_qstr(filepath, filepath - p);
    //     }

    //     set_sys_argv(argv, argc, 1);

    //     if (filepath != NULL) {
    //         pyexec_file(filepath);
    //     }
    // }

    // run boot-up scripts
    // pyexec_frozen_module("_boot.py");
    // do not delete this message, haas studio need use this message judge python status
    // printf(" ==== python execute bootpy ====\n");
    pyexec_file_if_exists("/boot.py");
    if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
        char *path = is_mainpy_exist();
        if (path != NULL) {
            // do not delete this message, haas studio need use this message judge python status
           // printf(" ==== python execute from %s ====\n", path);
            int ret = pyexec_file_if_exists(path);
            if (ret & PYEXEC_FORCED_EXIT) {
                goto soft_reset_exit;
            #if 1
            } else if (ret & PYEXEC_FORCED_QUIT) {
                goto soft_quit_exit;
            #endif
            }
        }
    }

    {
        for (;;) {
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                ret = pyexec_raw_repl();
                #if MICROPY_PY_AOS_QUIT
                if (ret == PYEXEC_FORCED_QUIT) {
                    goto soft_quit_exit;
                }
                #endif
                if (ret != 0) {
                    break;
                }
            } else {
                ret = pyexec_friendly_repl();
                #if MICROPY_PY_AOS_QUIT
                if (ret == PYEXEC_FORCED_QUIT) {
                    goto soft_quit_exit;
                }
                #endif
                if (ret != 0) {
                    break;
                }
            }
        }
    }

soft_reset_exit:
    mp_hal_stdout_tx_str("MPY: soft reboot\r\n");
    mp_deinit();

    #if MICROPY_PY_AOS_QUIT
    goto soft_reset;
    #endif

soft_quit_exit:
    return 0;
}

MP_WEAK mp_lexer_t *mp_lexer_new_from_file(const char *filename)
{
    mp_raise_OSError(MP_ENOENT);
}

void nlr_jump_fail(void *val)
{
    while (1)
        ;
}

void NORETURN __fatal_error(const int8_t *msg)
{
    while (1)
        ;
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr)
{
    printf("assert:%s:%d:%s: %s\n", file, line, func, expr);
    mp_raise_msg(&mp_type_AssertionError, MP_ERROR_TEXT("C-level assert"));
}
#endif

#ifdef AOS_COMP_CLI
#if BOARD_HAAS700
#include <console.h>
#endif
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(python_entry, python, start micropython)
ALIOS_CLI_CMD_REGISTER(handle_unzip_cmd, unzip, start unzip)
#endif
