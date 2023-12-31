// This configuration is for a generic ESP32C3 board with 4MiB (or more) of flash.

#define MICROPY_HW_BOARD_NAME               "ESP32C3_USB module"
#define MICROPY_HW_MCU_NAME                 "ESP32-C3"
#define MICROPY_SW_VENDOR_NAME              "HaaSPython"

#define MICROPY_HW_ENABLE_SDCARD            (0)
#define MICROPY_PY_MACHINE_DAC              (0)
#define MICROPY_PY_MACHINE_I2S              (0)
