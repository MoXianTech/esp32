/**
 ***************************************************************************************************
 * 实验简介
 * 实验名称：SD卡实验
 * 实验平台：正点原子 ESP32-S3 开发板
 * 实验目的：学习SDIO驱动SD卡的使用

 ***************************************************************************************************
 * 硬件资源及引脚分配
 * 1 LED
     LED - IO1
 * 2 按键
     BOOT - IO0 
 * 3 正点原子1.3/2.4寸SPILCD模块
 * 4 SD卡(SDIO驱动)

 ***************************************************************************************************
 * 实验现象
 * 1 程序运行后，若初始化SD卡成功，则会通过串口或者VsCode终端输出SD卡的相关参数，并且在LCD上显示SD卡的总容量和剩余容量
 * 2 LED闪烁，指示程序正在运行

 ***************************************************************************************************
 * 注意事项
 * USART1的通讯波特率为115200
 * 请使用XCOM串口调试助手，其他串口软件可能控制DTR、RST导致MCU复位、程序不运行
 * 开发板IO4引脚连接串口转TTL模块上的RXD，IO5引脚连接串口转TTL模块上的TXD，该模块再通过数据线连接至电脑上
 * 需将SD卡正确插入板载的SD卡槽，才能正常运行本实验例程

 ***********************************************************************************************************
 * 公司名称：广州市星翼电子科技有限公司（正点原子）
 * 电话号码：020-38271790
 * 传真号码：020-36773971
 * 公司网址：www.alientek.com
 * 购买地址：zhengdianyuanzi.tmall.com
 * 技术论坛：http://www.openedv.com/forum.php
 * 最新资料：www.openedv.com/docs/index.html
 *
 * 在线视频：www.yuanzige.com
 * B 站视频：space.bilibili.com/394620890
 * 公 众 号：mp.weixin.qq.com/s/y--mG3qQT8gop0VRuER9bw
 * 抖    音：douyin.com/user/MS4wLjABAAAAi5E95JUBpqsW5kgMEaagtIITIl15hAJvMO8vQMV1tT6PEsw-V5HbkNLlLMkFf1Bd
 ***********************************************************************************************************
 */