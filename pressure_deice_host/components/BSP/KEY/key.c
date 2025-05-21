#include "key.h"
#include "led.h"
#include "beep.h"


EventGroupHandle_t key_event_handle = NULL;
/**
 * @brief       外部中断服务函数
 * @param       arg：中断引脚号
 * @note        IRAM_ATTR: 这里的IRAM_ATTR属性用于将中断处理函数存储在内部RAM中，目的在于减少延迟
 * @retval      无
 */
static void IRAM_ATTR exit_gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;

    switch(gpio_num)
    {
        case KEY_GPIO_PIN_0:
            xEventGroupSetBitsFromISR(key_event_handle,
                                      (EventBits_t)KEY_EVENT_BIT_0,
                                      pdFAIL);
            break;
        case KEY_GPIO_PIN_1:
            xEventGroupSetBitsFromISR(key_event_handle,
                                      (EventBits_t)KEY_EVENT_BIT_1,
                                      pdFAIL);
            break;
        case KEY_GPIO_PIN_2:
            xEventGroupSetBitsFromISR(key_event_handle,
                                      (EventBits_t)KEY_EVENT_BIT_2,
                                      pdFAIL);
            break;
        default:
            break;
    }

    portYIELD_FROM_ISR(pdFAIL);
}

/**
 * @brief       外部中断初始化程序
 * @param       无
 * @retval      无
 */
void key_init(EventGroupHandle_t key_event)
{
    gpio_config_t gpio_init_struct;

    /* 配置BOOT引脚和外部中断 */
    gpio_init_struct.mode = GPIO_MODE_INPUT;                    /* 选择为输入模式 */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;           /* 上拉使能 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;      /* 下拉失能 */
    gpio_init_struct.intr_type = GPIO_INTR_NEGEDGE;             /* 下降沿触发 */
    gpio_init_struct.pin_bit_mask = 1ull << KEY_GPIO_PIN_0
                                    | 1ull << KEY_GPIO_PIN_1
                                    | 1ull << KEY_GPIO_PIN_2;  /* 配置BOOT按键引脚 */
    gpio_config(&gpio_init_struct);                             /* 配置使能 */

    key_event_handle = key_event;

    /* 注册中断服务 */
    gpio_install_isr_service(0);

    /* 设置GPIO的中断回调函数 */
    gpio_isr_handler_add(KEY_GPIO_PIN_0,
                         exit_gpio_isr_handler,
                         (void*) KEY_GPIO_PIN_0);

    gpio_isr_handler_add(KEY_GPIO_PIN_1,
                         exit_gpio_isr_handler,
                         (void*) KEY_GPIO_PIN_1);

    gpio_isr_handler_add(KEY_GPIO_PIN_2,
                         exit_gpio_isr_handler,
                         (void*) KEY_GPIO_PIN_2);

}

