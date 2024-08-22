#include "beep.h"
/**
 * @brief       初始化PWM
 * @param       resolution：PWM占空比分辨率
 *              freq: PWM信号频率
 * @retval      无
 */
void pwm_init(uint8_t resolution, uint16_t freq)
{
    ledc_timer_config_t ledc_timer;                 /* LEDC定时器句柄 */
    ledc_channel_config_t ledc_channel;             /* LEDC通道配置句柄 */

    /* 配置LEDC定时器 */
    ledc_timer.duty_resolution = resolution;        /* PWM占空比分辨率 */
    ledc_timer.freq_hz = freq;                      /* PWM信号频率 */
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;    /* 定时器模式 */
    ledc_timer.timer_num = LEDC_PWM_TIMER;          /* 定时器序号 */
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;             /* LEDC时钟源 */
    ledc_timer_config(&ledc_timer);                 /* 配置定时器 */

    /* 配置LEDC通道 */
    ledc_channel.gpio_num = LEDC_PWM_CH0_GPIO;      /* LED控制器通道对应引脚 */
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;  /* LEDC高速模式 */
    ledc_channel.channel = LEDC_PWM_CH0_CHANNEL;    /* LEDC控制器通道号 */
    ledc_channel.intr_type = LEDC_INTR_DISABLE;     /* LEDC失能中断 */
    ledc_channel.timer_sel = LEDC_PWM_TIMER;        /* 定时器序号 */
    ledc_channel.duty = 0;                          /* 占空比值 */
    ledc_channel_config(&ledc_channel);             /* 配置LEDC通道 */
}

/**
 * @brief       PWM占空比设置
 * @param       duty：PWM占空比
 * @retval      无
 */
void pwm_set_duty(uint16_t duty)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_PWM_CH0_CHANNEL, duty); /* 设置占空比 */
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_PWM_CH0_CHANNEL);    /* 更新占空比 */
}


void beep_init(void)
{
#if 0
    gpio_config_t gpio_init_struct = {0};

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* 失能引脚中断 */
    gpio_init_struct.mode = GPIO_MODE_OUTPUT;         /* 输入输出模式 */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_DISABLE;       /* 使能上拉 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_ENABLE;  /* 失能下拉 */
    gpio_init_struct.pin_bit_mask = 1ull << BEEP_GPIO_PIN;   /* 设置的引脚的位掩码 */
    gpio_config(&gpio_init_struct);                         /* 配置GPIO */

    gpio_set_level(BEEP_GPIO_PIN, 0);
#endif

    pwm_init(10,1000);
    pwm_set_duty(200);

}
