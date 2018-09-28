
#ifdef PT_SCANNER
const char TSL_STRING[] =
    "{\"schema\":\"https://iotx-tsl.oss-ap-southeast-1.aliyuncs.com/schema.json\",\"profile\":{\"productKey\":\"a1hPP1tOswI\"},\"services\":[{\"outputData\":[],\"identifier\":\"set\",\"inputData\":[],\"method\":\"thing.service.property.set\",\"name\":\"set\",\"required\":true,\"callType\":\"async\",\"desc\":\"属性设置\"},{\"outputData\":[{\"identifier\":\"GatewayStatus\",\"dataType\":{\"specs\":{\"0\":\"通信正常\",\"1\":\"通信失败\",\"2\":\"设备异常\"},\"type\":\"enum\"},\"name\":\"网关状态\"},{\"identifier\":\"GeoLocation\",\"dataType\":{\"specs\":[{\"identifier\":\"Longitude\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-180\",\"unitName\":\"度\",\"max\":\"180\"},\"type\":\"double\"},\"name\":\"经度\"},{\"identifier\":\"Latitude\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-90\",\"unitName\":\"度\",\"max\":\"90\"},\"type\":\"double\"},\"name\":\"纬度\"},{\"identifier\":\"Altitude\",\"dataType\":{\"specs\":{\"unit\":\"m\",\"min\":\"0\",\"unitName\":\"米\",\"max\":\"9999\"},\"type\":\"double\"},\"name\":\"海拔\"}],\"type\":\"struct\"},\"name\":\"地理位置\"}],\"identifier\":\"get\",\"inputData\":[\"GatewayStatus\",\"GeoLocation\"],\"method\":\"thing.service.property.get\",\"name\":\"get\",\"required\":true,\"callType\":\"async\",\"desc\":\"属性获取\"}],\"properties\":[{\"identifier\":\"GatewayStatus\",\"dataType\":{\"specs\":{\"0\":\"通信正常\",\"1\":\"通信失败\",\"2\":\"设备异常\"},\"type\":\"enum\"},\"name\":\"网关状态\",\"accessMode\":\"r\",\"required\":true},{\"identifier\":\"GeoLocation\",\"dataType\":{\"specs\":[{\"identifier\":\"Longitude\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-180\",\"unitName\":\"度\",\"max\":\"180\"},\"type\":\"double\"},\"name\":\"经度\"},{\"identifier\":\"Latitude\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-90\",\"unitName\":\"度\",\"max\":\"90\"},\"type\":\"double\"},\"name\":\"纬度\"},{\"identifier\":\"Altitude\",\"dataType\":{\"specs\":{\"unit\":\"m\",\"min\":\"0\",\"unitName\":\"米\",\"max\":\"9999\"},\"type\":\"double\"},\"name\":\"海拔\"}],\"type\":\"struct\"},\"name\":\"地理位置\",\"accessMode\":\"r\",\"required\":true}],\"events\":[{\"outputData\":[{\"identifier\":\"GatewayStatus\",\"dataType\":{\"specs\":{\"0\":\"通信正常\",\"1\":\"通信失败\",\"2\":\"设备异常\"},\"type\":\"enum\"},\"name\":\"网关状态\"},{\"identifier\":\"GeoLocation\",\"dataType\":{\"specs\":[{\"identifier\":\"Longitude\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-180\",\"unitName\":\"度\",\"max\":\"180\"},\"type\":\"double\"},\"name\":\"经度\"},{\"identifier\":\"Latitude\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-90\",\"unitName\":\"度\",\"max\":\"90\"},\"type\":\"double\"},\"name\":\"纬度\"},{\"identifier\":\"Altitude\",\"dataType\":{\"specs\":{\"unit\":\"m\",\"min\":\"0\",\"unitName\":\"米\",\"max\":\"9999\"},\"type\":\"double\"},\"name\":\"海拔\"}],\"type\":\"struct\"},\"name\":\"地理位置\"}],\"identifier\":\"post\",\"method\":\"thing.event.property.post\",\"name\":\"post\",\"type\":\"info\",\"required\":true,\"desc\":\"属性上报\"},{\"outputData\":[{\"identifier\":\"DeviceNumber\",\"dataType\":{\"specs\":{\"length\":\"256\"},\"type\":\"text\"},\"name\":\"设备编号\"},{\"identifier\":\"Contain\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"name\":\"扫码内容\"},{\"identifier\":\"Timestamp\",\"dataType\":{\"specs\":{\"length\":\"128\"},\"type\":\"text\"},\"name\":\"时间\"}],\"identifier\":\"scanevent\",\"method\":\"thing.event.scanevent.post\",\"name\":\"扫码\",\"type\":\"info\",\"required\":false}]}";
#else
const char TSL_STRING[] =
    "{\"schema\":\"https://iotx-tsl.oss-ap-southeast-1.aliyuncs.com/schema.json\",\"profile\":{\"productKey\":\"a1FWvP18TBY\"},\"services\":[{\"outputData\":[],\"identifier\":\"set\",\"inputData\":[{\"identifier\":\"Accelerometer\",\"dataType\":{\"specs\":[{\"identifier\":\"x\",\"dataType\":{\"specs\":{\"min\":\"-10\",\"unitName\":\"无\",\"max\":\"10\"},\"type\":\"float\"},\"name\":\"X\"},\
    {\"identifier\":\"y\",\"dataType\":{\"specs\":{\"min\":\"-10\",\"unitName\":\"无\",\"max\":\"10\"},\"type\":\"float\"},\"name\":\"Y\"},{\"identifier\":\"z\",\"dataType\":{\"specs\":{\"min\":\"-10\",\"unitName\":\"无\",\"max\":\"10\"},\"type\":\"float\"},\"name\":\"Z\"}],\"type\":\"struct\"},\"name\":\"Accelerometer\"},{\"identifier\":\"Gyroscope\",\"dataType\":{\"specs\":[{\"identifier\":\"x_dps\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-2000\",\"unitName\":\"度\",\"max\":\"2000\"}\
    ,\"type\":\"float\"},\"name\":\"X\"},{\"identifier\":\"y_dps\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-2000\",\"unitName\":\"度\",\"max\":\"2000\"},\"type\":\"float\"},\"name\":\"Y\"},{\"identifier\":\"z_dps\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-2000\",\"unitName\":\"度\",\"max\":\"2000\"},\"type\":\"float\"},\"name\":\"Z\"}],\"type\":\"struct\"},\"name\":\"Gyroscope\"},{\"identifier\":\"CurrentTemperature\",\"dataType\":{\"specs\":{\"min\":\"-40\",\"unitName\":\"无\",\"max\":\"80\"},\"type\":\"float\"}\
    ,\"name\":\"CurrentTemperature\"},{\"identifier\":\"CurrentHumidity\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"100\"},\"type\":\"float\"},\"name\":\"CurrentHumidity\"},{\"identifier\":\"RTCTime\",\"dataType\":{\"specs\":[{\"identifier\":\"year\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"99\"},\"type\":\"int\"},\"name\":\"year\"},{\"identifier\":\"month\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"12\"},\"type\":\"int\"},\"name\":\"month\"},{\"identifier\":\"date\",\"dataType\":{\"specs\":{\"min\":\"1\",\"unitName\":\"无\",\"max\":\"31\"},\"type\":\"int\"},\"name\":\"date\"},{\"identifier\":\"day\",\"dataType\":{\"specs\":{\"min\":\"1\",\"unitName\":\"无\",\"max\":\"7\"},\"type\":\"int\"},\"name\":\"day\"},{\"identifier\":\"hours\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"23\"},\"type\":\"int\"},\"name\":\"hours\"},{\"identifier\":\"minutes\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"59\"},\"type\":\"int\"},\"name\":\"minutes\"},{\"identifier\":\"seconds\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"59\"},\"type\":\"int\"},\"name\":\"seconds\"}],\"type\":\"struct\"},\"name\":\"RTCTime\"}],\"method\":\"thing.service.property.set\",\"name\":\"set\",\"required\":true,\"callType\":\"async\",\"desc\":\"属性设置\"},{\"outputData\":[{\"identifier\":\"Accelerometer\",\"dataType\":{\"specs\":[{\"identifier\":\"x\",\"dataType\":{\"specs\":{\"min\":\"-10\",\"unitName\":\"无\",\"max\":\"10\"},\"type\":\"float\"},\"name\":\"X\"},{\"identifier\":\"y\",\"dataType\":{\"specs\":{\"min\":\"-10\",\"unitName\":\"无\",\"max\":\"10\"},\"type\":\"float\"},\"name\":\"Y\"},{\"identifier\":\"z\",\"dataType\":{\"specs\":{\"min\":\"-10\",\"unitName\":\"无\",\"max\":\"10\"},\"type\":\"float\"},\"name\":\"Z\"}],\"type\":\"struct\"},\"name\":\"Accelerometer\"},{\"identifier\":\"Gyroscope\",\"dataType\":{\"specs\":[{\"identifier\":\"x_dps\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-2000\",\"unitName\":\"度\",\"max\":\"2000\"},\"type\":\"float\"},\"name\":\"X\"},{\"identifier\":\"y_dps\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-2000\",\"unitName\":\"度\",\"max\":\"2000\"},\"type\":\"float\"},\"name\":\"Y\"},{\"identifier\":\"z_dps\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-2000\",\"unitName\":\"度\",\"max\":\"2000\"},\"type\":\"float\"},\"name\":\"Z\"}],\"type\":\"struct\"},\"name\":\"Gyroscope\"},{\"identifier\":\"CurrentTemperature\",\"dataType\":{\"specs\":{\"min\":\"-40\",\"unitName\":\"无\",\"max\":\"80\"},\"type\":\"float\"},\"name\":\"CurrentTemperature\"},{\"identifier\":\"CurrentHumidity\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"100\"},\"type\":\"float\"},\"name\":\"CurrentHumidity\"},{\"identifier\":\"RTCTime\",\"dataType\":{\"specs\":[{\"identifier\":\"year\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"99\"},\"type\":\"int\"},\"name\":\"year\"},{\"identifier\":\"month\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"12\"},\"type\":\"int\"},\"name\":\"month\"},{\"identifier\":\"date\",\"dataType\":{\"specs\":{\"min\":\"1\",\"unitName\":\"无\",\"max\":\"31\"},\"type\":\"int\"},\"name\":\"date\"},{\"identifier\":\"day\",\"dataType\":{\"specs\":{\"min\":\"1\",\"unitName\":\"无\",\"max\":\"7\"},\"type\":\"int\"},\"name\":\"day\"},{\"identifier\":\"hours\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"23\"},\"type\":\"int\"},\"name\":\"hours\"},{\"identifier\":\"minutes\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"59\"},\"type\":\"int\"},\"name\":\"minutes\"},{\"identifier\":\"seconds\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"59\"},\"type\":\"int\"},\"name\":\"seconds\"}],\"type\":\"struct\"},\"name\":\"RTCTime\"}],\"identifier\":\"get\",\"inputData\":[\"Accelerometer\",\"Gyroscope\",\"CurrentTemperature\",\"CurrentHumidity\",\"RTCTime\"],\"method\":\"thing.service.property.get\",\"name\":\"get\",\"required\":true,\"callType\":\"async\",\"desc\":\"属性获取\"}]\
    ,\"properties\":[{\"identifier\":\"Accelerometer\",\"dataType\":{\"specs\":[{\"identifier\":\"x\",\"dataType\":{\"specs\":{\"min\":\"-10\",\"unitName\":\"无\",\"max\":\"10\"},\"type\":\"float\"},\"name\":\"X\"},{\"identifier\":\"y\",\"dataType\":{\"specs\":{\"min\":\"-10\",\"unitName\":\"无\",\"max\":\"10\"},\"type\":\"float\"},\"name\":\"Y\"},{\"identifier\":\"z\",\"dataType\":{\"specs\":{\"min\":\"-10\",\"unitName\":\"无\",\"max\":\"10\"},\"type\":\"float\"},\"name\":\"Z\"}],\"type\":\"struct\"},\"name\":\"Accelerometer\",\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"Gyroscope\",\"dataType\":{\"specs\":[{\"identifier\":\"x_dps\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-2000\",\"unitName\":\"度\",\"max\":\"2000\"},\"type\":\"float\"},\"name\":\"X\"},{\"identifier\":\"y_dps\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-2000\",\"unitName\":\"度\",\"max\":\"2000\"},\"type\":\"float\"},\"name\":\"Y\"},{\"identifier\":\"z_dps\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-2000\",\"unitName\":\"度\",\"max\":\"2000\"},\"type\":\"float\"},\"name\":\"Z\"}],\"type\":\"struct\"},\"name\":\"Gyroscope\",\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"CurrentTemperature\",\"dataType\":{\"specs\":{\"min\":\"-40\",\"unitName\":\"无\",\"max\":\"80\"},\"type\":\"float\"},\"name\":\"CurrentTemperature\",\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"CurrentHumidity\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"100\"},\"type\":\"float\"},\"name\":\"CurrentHumidity\",\"accessMode\":\"rw\",\"required\":true},{\"identifier\":\"RTCTime\",\"dataType\":{\"specs\":[{\"identifier\":\"year\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"99\"},\"type\":\"int\"},\"name\":\"year\"},{\"identifier\":\"month\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"12\"},\"type\":\"int\"},\"name\":\"month\"},{\"identifier\":\"date\",\"dataType\":{\"specs\":{\"min\":\"1\",\"unitName\":\"无\",\"max\":\"31\"},\"type\":\"int\"},\"name\":\"date\"},{\"identifier\":\"day\",\"dataType\":{\"specs\":{\"min\":\"1\",\"unitName\":\"无\",\"max\":\"7\"},\"type\":\"int\"},\"name\":\"day\"},{\"identifier\":\"hours\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"23\"},\"type\":\"int\"},\"name\":\"hours\"},{\"identifier\":\"minutes\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"59\"},\"type\":\"int\"},\"name\":\"minutes\"},{\"identifier\":\"seconds\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"59\"},\"type\":\"int\"},\"name\":\"seconds\"}],\"type\":\"struct\"},\"name\":\"RTCTime\",\"accessMode\":\"rw\",\"required\":false}],\"events\":[{\"outputData\":[{\"identifier\":\"Accelerometer\",\"dataType\":{\"specs\":[{\"identifier\":\"x\",\"dataType\":{\"specs\":{\"min\":\"-10\",\"unitName\":\"无\",\"max\":\"10\"},\"type\":\"float\"},\"name\":\"X\"},{\"identifier\":\"y\",\"dataType\":{\"specs\":{\"min\":\"-10\",\"unitName\":\"无\",\"max\":\"10\"},\"type\":\"float\"},\"name\":\"Y\"},{\"identifier\":\"z\",\"dataType\":{\"specs\":{\"min\":\"-10\",\"unitName\":\"无\",\"max\":\"10\"},\"type\":\"float\"},\"name\":\"Z\"}],\"type\":\"struct\"},\"name\":\"Accelerometer\"},{\"identifier\":\"Gyroscope\"\
    ,\"dataType\":{\"specs\":[{\"identifier\":\"x_dps\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-2000\",\"unitName\":\"度\",\"max\":\"2000\"},\"type\":\"float\"},\"name\":\"X\"},{\"identifier\":\"y_dps\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-2000\",\"unitName\":\"度\",\"max\":\"2000\"},\"type\":\"float\"},\"name\":\"Y\"},{\"identifier\":\"z_dps\",\"dataType\":{\"specs\":{\"unit\":\"°\",\"min\":\"-2000\",\"unitName\":\"度\",\"max\":\"2000\"},\"type\":\"float\"},\"name\":\"Z\"}],\"type\":\"struct\"},\"name\":\"Gyroscope\"},{\"identifier\":\"CurrentTemperature\",\"dataType\":{\"specs\":{\"min\":\"-40\",\"unitName\":\"无\",\"max\":\"80\"},\"type\":\"float\"},\"name\":\"CurrentTemperature\"},{\"identifier\":\"CurrentHumidity\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"100\"},\"type\":\"float\"},\"name\":\"CurrentHumidity\"},{\"identifier\":\"RTCTime\",\"dataType\":{\"specs\":[{\"identifier\":\"year\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"99\"},\"type\":\"int\"},\"name\":\"year\"},{\"identifier\":\"month\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"12\"},\"type\":\"int\"},\"name\":\"month\"},{\"identifier\":\"date\",\"dataType\":{\"specs\":{\"min\":\"1\",\"unitName\":\"无\",\"max\":\"31\"},\"type\":\"int\"},\"name\":\"date\"},{\"identifier\":\"day\",\"dataType\":{\"specs\":{\"min\":\"1\",\"unitName\":\"无\",\"max\":\"7\"},\"type\":\"int\"},\"name\":\"day\"},{\"identifier\":\"hours\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"23\"},\"type\":\"int\"},\"name\":\"hours\"},{\"identifier\":\"minutes\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"59\"},\"type\":\"int\"},\"name\":\"minutes\"},{\"identifier\":\"seconds\",\"dataType\":{\"specs\":{\"min\":\"0\",\"unitName\":\"无\",\"max\":\"59\"},\"type\":\"int\"},\"name\":\"seconds\"}],\"type\":\"struct\"},\"name\":\"RTCTime\"}],\"identifier\":\"post\",\"method\":\"thing.event.property.post\",\"name\":\"post\",\"type\":\"info\",\"required\":true,\"desc\":\"属性上报\"}]}";
#endif
