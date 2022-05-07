var iot = require('iot');
var network = require('network');
var net = network.openNetWorkClient({devType: 'wifi'});

var productKey = 'xxx';     // 替换成用户网关的pk
var deviceName = 'xxx-01';  // 替换成用户网关的dn
var deviceSecret = 'xxx';   // 替换成用户网关的ds

var lightSwitch = 0;
var gateway;

var subdev = [
    {
        productKey: 'xxx',   // 替换成用户子设备的pk
        deviceName: 'xxx'    // 替换成用户子设备的pk
    }
];

/* post property */
function postProperty(id, devInfo, content) {
    var topic = '/sys/' + devInfo.productKey + '/' + devInfo.deviceName + '/thing/event/property/post';
    var payload = JSON.stringify({
        id: id,
        version: '1.0',
        params: content,
        method: 'thing.event.property.post'
    });

    gateway.publish({
        topic: topic,
        payload: payload,
        qos: 1
    });
}

/* post event */
function postEvent(id, devInfo, eventId, content) {
    var topic = '/sys/' + devInfo.productKey + '/' + devInfo.deviceName + '/thing/event/' + eventId + '/post';
    var payload = JSON.stringify({
        id: id,
        version: '1.0',
        params: content,
        method: 'thing.event.' + eventId + '.post'
    });

    gateway.publish({
        topic: topic,
        payload: payload,
        qos: 1
    });
}

function createGateway() {
    gateway = iot.gateway({
        productKey: productKey,
        deviceName: deviceName,
        deviceSecret: deviceSecret
    });

    gateway.on('connect', function () {
        console.log('(re)connected');
        gateway.getNtpTime(function(res) {
            console.log('ntp time ' + res.year + '-' + res.month + '-' + res.day + ' ' + res.hour + ':' + res.minute + ':' + res.second + '.' + res.msecond + ', timestamp ' + res.timestamp);
        });

        gateway.registerSubDevice(subdev, function (res) {
            console.log('reg subdev resp: ');
            console.log('code: ' + res.code);
            console.log('data: ' + res.data);
            console.log('message: ' + res.message);
            var obj = JSON.parse(res.data);
            for (var i = 0; i < obj.length; i++) {
                console.log('add device: ' + obj[i].productKey + ', '+ obj[i].deviceName + ', '+ obj[i].deviceSecret);
                gateway.addTopo([{
                    productKey: obj[i].productKey,
                    deviceName: obj[i].deviceName,
                    deviceSecret: obj[i].deviceSecret
                }]);
                gateway.login([{
                    productKey: obj[i].productKey,
                    deviceName: obj[i].deviceName,
                    deviceSecret: obj[i].deviceSecret
                }]);
            }
        });
    });

    /* 网络断开事件 */
    gateway.on('disconnect', function () {
        console.log('disconnect ');
    });

    /* mqtt消息 */
    gateway.on('message', function (res) {
        /* 通过 topic中的pk和dn信息，判断是对哪个子设备的调用 */
        var pk = res.topic.split('/')[2];
        var dn = res.topic.split('/')[3];
        console.log('mqtt message')
        console.log('mqtt topic is ' + res.topic);
        console.log('PK: ' + pk)
        console.log('DN: ' + dn)
        console.log('mqtt payload is ' + res.payload);
    })

    /* 关闭连接事件 */
    gateway.on('end', function () {
        console.log('iot client just closed');
    });

    /* 发生错误事件 */
    gateway.on('error', function (err) {
        console.log('error ' + err);
    });
}


console.log('====== create aiot gateway ======');
/**
 * 获取网络类型
 * 目前支持两种类型：wifi cellular（蜂窝网）
 */
var netType = net.getType();
console.log('net type is: ' + netType);

/**
 * 获取网络状态
 * 目前支持两种状态：connect disconnect
 */
var netStatus = net.getStatus();
console.log('net status is: ' + netStatus);

if (netStatus == 'connect') {
    /* 网络状态已连接，获取网络状态 */
    console.log('net status is connect');
    createGateway();
} else {
    /* 网络状态未连接，如果是wifi设备则主动联网(4G模组中系统会自动注网) */
    if (netType === "wifi") {
        net.connect({
            ssid: 'xxx',              //请替换为自己的热点ssid
            password: 'xxx'           //请替换为自己热点的密码
        });
    }

    /**
     * 监听网络连接成功事件，成功执行回调函数
     */
    net.on('connect', function () {
        console.log('network connected');
        createGateway();
    });

    net.on('disconnect', function () {
     console.log('network disconnected');
 });
}



