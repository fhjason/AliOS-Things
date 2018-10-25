src     = ['board.c']

component = aos_board_component('board_mk3060', 'moc108', src)
aos_global_config.add_ld_files('memory.ld.S')

build_types=""

supported_targets="alinkapp helloworld linuxapp meshapp tls udataapp networkapp acapp linkkitapp mqttapp coapapp linkkit_gateway"
linux_only_targets="itls_app sst_app nano wifihalapp helloworld_nocli wifimonitor blink id2_app vflashdemo prov_app netmgrapp hdlcapp.hdlcserver athostapp kernel_test i2c_hts221_test otaapp ldapp cryptotest yts http2app"
