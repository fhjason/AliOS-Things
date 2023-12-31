set(IDF_TARGET esp32)

set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.ble
    boards/sdkconfig.spiram
    boards/sdkconfig.240mhz
    boards/M5STACK_CORE2/sdkconfig.board
    boards/M5STACK_CORE2/sdkconfig.lvgl
)

if(NOT MICROPY_FROZEN_MANIFEST)
   set(MICROPY_FROZEN_MANIFEST ${MICROPY_PORT_DIR}/boards/manifest.py)
endif()
