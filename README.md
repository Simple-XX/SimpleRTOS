# SimpleRTOS(catOS)

## 简介

一个简单的嵌入式实时操作系统
- 抢占式优先级调度
- 可移植层
- 重写更简易更轻量级的部分库函数
- 除内核外还提供部分组件
  - 简易的shell程序
  - 设备驱动框架
  - 内存地址读写功能
- 通用性高的makefile编译框架


## 开发环境

### 软件环境
---
交叉编译调试工具 `gcc-arm-none-eabi`

项目构建        `makefile`

pc端调试工具    `openocd`

`$sudo spt install openocd`

### 硬件环境
---
硬件开发板      `stm32f103c8t6最小系统板`

调试器         `cmsis-dap调试器或其他调试器`

其他硬件工具    `ttl转串口`


## 构建与运行

step1: 打开一个终端(终端1, 编译)
```shell
$git clone https://github.com/Simple-XX/SimpleRTOS.git
$cd SimpleRTOS/workspace
$make p=demo
```
编译完成后的生成文件位于workspace/output/demo目录下

如果你想：

- 调试->请看step2

- 只是运行->请看step4.1

--------------调试请从这里继续-------------------

step2: 另外打开一个终端(终端2, openocd连接调试器)
```shell
$openocd -f /usr/share/openocd/scripts/interface/cmsis-dap.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg
#interface下有对不同调试器的支持
```
step3: 另外打开一个终端(终端3, 打开串口)
```shell
$minicom -D /dev/ttyACM0
#参数根据转串口驱动设备决定
#然后设置波特率
$[Ctrl-A]
$z
$o
->Serial port setup
A -    Serial Device      : /dev/ttyACM0
B - Lockfile Location     : /var/lock   
C -   Callin Program      :             
D -  Callout Program      :             
E -    Bps/Par/Bits       : 115200 8N1  
F - Hardware Flow Control : No          
G - Software Flow Control : No
```
修改完成后最好再次打开设置看是否保存设置成功

step4: 另外打开一个终端(终端4, 使用gdb与openocd配合下载并运行程序)
```shell
$../gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-gdb ./output/demo/bin/demo.elf
$sudo target remote localhost:3333
$load
$continue
```
--------------运行请从这里继续-------------------

step4.1:修改workspace/flash.sh中openocd为你的openocd安装目录

step4.2:使用openode下载程序到开发板并自动复位运行
```cmd
$./flash.sh
```

step5: 随即在终端3即可观察到串口输出
```
[cat_shell_port] cat_shell init success 
_____________________________________
     ___      _   ___ _        _ _   
    / __|__ _| |_/ __| |_  ___| | |  
   | (__/ _` |  _\__ \ ' \/ -_) | |
    \___\__,_|\__|___/_||_\___|_|_| 
 _____________________________________

 CatShell v1.0.1 created by mio 
 type "help" to get more info

cat>
```

## 目录结构


## 版权信息

此项目使用 MIT 许可证
