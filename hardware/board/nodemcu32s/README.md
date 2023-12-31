# ESP32开发板介绍

## 一、概述
ESP32 是集成2.4 GHz Wi-Fi 和蓝牙双模的单芯片方案，采用台积电(TSMC) 超低功耗的40 纳米工艺，具有超
高的射频性能、稳定性、通用性和可靠性，以及超低的功耗，满足不同的功耗需求，适用于各种应用场景。
目前ESP32 系列的产品型号包括ESP32-D0WD-V3，ESP32-D0WDQ6-V3，ESP32-D0WD，ESP32-D0WDQ6，
ESP32-D2WD，ESP32-S0WD 和ESP32-U4WDH，其中ESP32-D0WD-V3，ESP32-D0WDQ6-V3 和ESP32-
U4WDH 是基于ECO V3 的芯片型号。

### 1、超低功耗
ESP32 专为移动设备、可穿戴电子产品和物联网(IoT) 应用而设计。作为业内领先的低功耗芯片，ESP32 具有精
细的时钟门控、省电模式和动态电压调整等特性。
例如，在低功耗IoT 传感器Hub 应用场景中，ESP32 只有在特定条件下才会被周期性地唤醒。低占空比可以极
大降低ESP32 芯片的能耗。射频功率放大器的输出功率也可调节，以实现通信距离、数据率和功耗之间的最佳
平衡。

### 2、高集成度
ESP32 是业内领先的高度集成的Wi-Fi+ 蓝牙解决方案，外部元器件只需大约20 个。ESP32 集成了天线开关、射
频Balun、功率放大器、低噪声放大器、滤波器以及电源管理模块，极大减少了印刷电路板(PCB) 的面积。
ESP32 采用CMOS 工艺实现单芯片集成射频和基带，还集成了先进的自校准电路，实现了动态自动调整，可以
消除外部电路的缺陷，更好地适应外部环境的变化。因此，ESP32 的批量生产可以不需要昂贵的专用Wi-Fi 测
试设备。

## 二、硬件资源
### 1、开发板资源介绍
#### （1） 主板配置
ESP32-32S开发板内置蓝牙和Wi-Fi功能，板载USB转TTL芯片，外形大小合理：48.26mm * 25.4mm ，具体功能定义如下表：

| 名称 | 数量 | 描述 |
| :- | :-: |  :- |
| 核心板 | 1 |  ESP32-32S |
| TTL芯片 | 1 |  型号：CP2102/CH340 |
| 指示灯 | 1 |  红色电源指示灯 |
| 按键 | 2 | 1个BOOT，1个可编程按键 |
| USB接口 | 1 |  Micro USB，烧录/调试 |




#### （2）扩展接口
下图是ESP32开发板扩展接口，共有38个引脚，其中2个电源，分别是5V和3.3V,3个GND,其余32个是GPIO。除了GPIO34、35、36、39只能作为输入外，其他的GPIO都可以支持PWM输出，多数GPIO还复用了其他功能，比如：IIC、ADC、SPI、UART、DAC等功能。并且每路GPIO的最大驱动电流是10mA，且每个GPIO引脚的额定电流是6mA，最大电流为12mA。

#### （3）电气特性
| 工作电压 | 3.5v~5v |
| :- | :- |
|充电电流| 450mA，可充电锂电池|
|工作温度|-20～85度|
|环境温度|5～85%RH（无凝结)|
