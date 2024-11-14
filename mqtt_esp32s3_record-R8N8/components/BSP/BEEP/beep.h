#ifndef __BEEP_H__
#define __BEEP_H__

#include "driver/gpio.h"
#include "driver/ledc.h"

#define BEEP_GPIO_PIN GPIO_NUM_10

/* 引脚以及重要参数定义 */
#define LEDC_PWM_TIMER          LEDC_TIMER_1        /* 使用定时器0 */
#define LEDC_PWM_CH0_GPIO       GPIO_NUM_10         /* LED控制器通道对应GPIO */
#define LEDC_PWM_CH0_CHANNEL    LEDC_CHANNEL_1      /* LED控制器通道号 */


void beep_trigger(bool value);
void beep_init(void);
void beep_set_freq(uint16_t freq);


#endif
