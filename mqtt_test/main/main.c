/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       WIFI Aliyun实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 ESP32-S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "led.h"
#include "lcd.h"
#include "wifi_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "iic.h"
#include "ap3216c.h"
#include "AHT20.h"
#include "thread_init.h"


i2c_obj_t i2c0_master, i2c1_master;

/**
 * @brief       程序入口
 * @param       无
 * @retval      无
 */
void app_main(void)
{
    esp_err_t ret;

    ret = nvs_flash_init();             /* 初始化NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                         /* 初始化LED */
    i2c0_master = iic_init(I2C_NUM_0);  /* 初始化IIC0 */
    spi2_init();                        /* 初始化SPI2 */
    xl9555_init(i2c0_master);           /* IO扩展芯片初始化 */
    lcd_init();                         /* 初始化LCD */
    ap3216c_init(i2c0_master);                          /* 初始化AP3216C */

    thraed_work_init();
}
