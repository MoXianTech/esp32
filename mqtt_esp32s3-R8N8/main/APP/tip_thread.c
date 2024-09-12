#include "thread_init.h"
#include "tip_thread.h"
#include "beep.h"
#include "led.h"
#include "key.h"
#include "string.h"

void tip_thread(void *pvparams)
{
    thread_pvparam_t *thread_pvparam = (thread_pvparam_t *)pvparams;
    uint16_t freq_count = 200;
    bool beep_enable = false;
    uint64_t event_bit = 0x00;

    thread_pvparam->key_event = xEventGroupCreate();
    led_init();
    beep_init();
    key_init(thread_pvparam->key_event);


    ESP_LOGI(__FUNCTION__, "thread start!");
    while(1)
    {
        if (beep_enable) 
        {
            beep_trigger(1);
            beep_set_freq(freq_count);
        } else {
            beep_trigger(0);
        }

        LED_TOGGLE();
        
        event_bit = xEventGroupWaitBits(thread_pvparam->key_event,
                            (EventBits_t)KEY_EVENT_BIT_0 | KEY_EVENT_BIT_1 | KEY_EVENT_BIT_2,
                            pdTRUE,
                            pdFAIL,
                            10);


        switch(event_bit)
        {
            case KEY_EVENT_BIT_0:
                beep_enable = true;
                freq_count = 500;
                break;
            case KEY_EVENT_BIT_1:
                beep_enable = true;
                freq_count = 600;
                break;
            case KEY_EVENT_BIT_2:
                beep_enable = true;
                freq_count = 700;
                break;

            default:
                beep_enable = false;
                break;
        }

//        LED_TOGGLE();
//        vTaskDelay(10);
    }
}

