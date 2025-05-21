#include "rs2251_usart.h"

void rs2251_usart_init()
{
    gpio_config_t gpio_init_struct = {0};
    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* 失能引脚中断 */
    gpio_init_struct.mode = GPIO_MODE_INPUT_OUTPUT;         /* 输入输出模式 */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;       /* 使能上拉 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* 失能下拉 */
    gpio_init_struct.pin_bit_mask = 1ull << VCC_USART_IO_0 | 1ull << VCC_USART_IO_1 |
                                    1ull << VCC_USART_IO_2 | 1ull << VCC_USART_IO_3 |
                                    1ull << VCC_USART_IO_4 | 1ull << VCC_USART_IO_5 |
                                    1ull << VCC_USART_IO_6 | 1ull << VCC_USART_IO_7 |
                                    1ull << RS2251_USART_CTRL_A | 1ull << RS2251_USART_CTRL_B|
                                    1ull << RS2251_USART_CTRL_C;
    gpio_config(&gpio_init_struct);                         /* 配置GPIO */
}

uint8_t rs2251_range_num[] = {3, 0, 1, 2, 5, 7, 6, 4};
void rs2251_channel_ctrl(RS2251_USART_NUM_T rs2251_usart_num)
{
    uint8_t channel = 0;
    channel = rs2251_range_num[rs2251_usart_num];
    gpio_set_level(RS2251_USART_CTRL_A, channel & 0x01);
    gpio_set_level(RS2251_USART_CTRL_B, channel & 0x02);
    gpio_set_level(RS2251_USART_CTRL_C, channel & 0x04);
}

void rs2251_usart_ctrl_channel(RS2251_USART_NUM_T rs2251_usart_num, bool power_enable, bool usart_change)
{
 
    if (usart_change)
        rs2251_channel_ctrl(rs2251_usart_num);
    switch(rs2251_usart_num)
    {
        case USART_NUM_0:
            gpio_set_level(VCC_USART_IO_0, power_enable);
            break;
        case USART_NUM_1:
            gpio_set_level(VCC_USART_IO_1, power_enable);
            break;
        case USART_NUM_2:
            gpio_set_level(VCC_USART_IO_2, power_enable);
            break;
        case USART_NUM_3:
            gpio_set_level(VCC_USART_IO_3, power_enable);
            break;
        case USART_NUM_4:
            gpio_set_level(VCC_USART_IO_4, power_enable);
            break;
        case USART_NUM_5:
            gpio_set_level(VCC_USART_IO_5, power_enable);
            break;
        case USART_NUM_6:
            gpio_set_level(VCC_USART_IO_6, power_enable);
            break;
        case USART_NUM_7:
            gpio_set_level(VCC_USART_IO_7, power_enable);
            break;

        default:
            break;
    }
}


