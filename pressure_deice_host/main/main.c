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
#include "beep.h"
#include "demo_task.h"


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
    spi2_init();                        /* 初始化SPI2 */
#ifdef ENABLE_LCD
    lcd_init();
#endif

    while(1) {
        demo_test();
        vTaskDelay(1000);
    }

    thraed_work_init();
}
