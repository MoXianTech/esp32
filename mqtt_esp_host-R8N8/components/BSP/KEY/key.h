#ifndef __KEY_H__
#define __KEY_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


/* 引脚定义 */
#define KEY_GPIO_PIN_2   GPIO_NUM_35
#define KEY_GPIO_PIN_1   GPIO_NUM_36
#define KEY_GPIO_PIN_0   GPIO_NUM_37

#define KEY_EVENT_BIT_0 0x01
#define KEY_EVENT_BIT_1 0x02
#define KEY_EVENT_BIT_2 0x04

/* 函数声明 */
void key_init(EventGroupHandle_t key_event);

#endif
