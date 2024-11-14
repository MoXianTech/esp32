#ifndef __RS2251_USART_H__
#define __RS2251_USART_H__

#include "driver/gpio.h"


#define VCC_USART_IO_0 GPIO_NUM_38
#define VCC_USART_IO_1 GPIO_NUM_39
#define VCC_USART_IO_2 GPIO_NUM_41
#define VCC_USART_IO_3 GPIO_NUM_42
#define VCC_USART_IO_4 GPIO_NUM_14
#define VCC_USART_IO_5 GPIO_NUM_47
#define VCC_USART_IO_6 GPIO_NUM_48
#define VCC_USART_IO_7 GPIO_NUM_45


#define RS2251_USART_CTRL_A   GPIO_NUM_9
#define RS2251_USART_CTRL_B   GPIO_NUM_46
#define RS2251_USART_CTRL_C   GPIO_NUM_3

typedef enum {
    USART_NUM_0 = 0,
    USART_NUM_1,
    USART_NUM_2,
    USART_NUM_3,
    USART_NUM_4,
    USART_NUM_5,
    USART_NUM_6,
    USART_NUM_7
} RS2251_USART_NUM_T;

void rs2251_usart_init();
void rs2251_usart_ctrl_channel(RS2251_USART_NUM_T rs2251_usart_num, bool power_enable, bool usart_change);

#endif
