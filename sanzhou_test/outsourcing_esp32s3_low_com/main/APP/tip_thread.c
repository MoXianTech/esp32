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
    uint64_t time_count = 0x00;
    bool beep_toggle_enable = false, beep_toggle_flag = false;

    thread_pvparam->key_event = xEventGroupCreate();
    thread_pvparam->tip_event = xEventGroupCreate();
    led_init();
    beep_init();
    key_init(thread_pvparam->key_event);

    ESP_LOGI(__FUNCTION__, "thread start!");
    while(1)
    {
        if (beep_enable | beep_toggle_enable) 
        {
            beep_set_freq(freq_count);
            if (beep_toggle_enable && !beep_toggle_flag)
            {
                beep_toggle_flag = true;
                beep_trigger(0);
            } else {
                beep_toggle_flag = false;
                beep_trigger(1);
            }
            
        } else {
            beep_trigger(0);
        }

        LED_TOGGLE();
        
        event_bit = xEventGroupWaitBits(thread_pvparam->key_event,
                            KEY_EVENT_BIT_0 | KEY_EVENT_BIT_1 | KEY_EVENT_BIT_2,
                            pdTRUE,
                            pdFAIL,
                            0);


        switch(event_bit)
        {
            case KEY_EVENT_BIT_0:
                xEventGroupSetBits(thread_pvparam->tip_event,
                                   TIP_EVENT_BIT_1);
                break;
            case KEY_EVENT_BIT_1:
                xEventGroupSetBits(thread_pvparam->tip_event,
                                   TIP_EVENT_BIT_2);
                break;
            case KEY_EVENT_BIT_2:
                xEventGroupSetBits(thread_pvparam->tip_event,
                                   TIP_EVENT_BIT_3);
                break;

            default:
                break;
        }

        event_bit = xEventGroupWaitBits(thread_pvparam->tip_event,
                                        TIP_EVENT_BIT_0 \
                                        | TIP_EVENT_BIT_1 \
                                        | TIP_EVENT_BIT_2 \
                                        | TIP_EVENT_BIT_3 \
                                        | TIP_EVENT_BIT_4 \
                                        ,
                                        pdTRUE,
                                        pdFAIL,
                                        0);

        switch(event_bit)
        {
            case TIP_EVENT_BIT_0:
                beep_toggle_enable = true;
                freq_count = 500;
                break;
            case TIP_EVENT_BIT_1:
                beep_enable = true;
                freq_count = 500;
                break;
            case TIP_EVENT_BIT_2:
                beep_enable = true;
                freq_count = 600;
                break;
            case TIP_EVENT_BIT_3:
                beep_enable = true;
                freq_count = 700;
                break;
            case TIP_EVENT_BIT_4:
                beep_toggle_enable = false;
                break;
            default:
                if (!beep_toggle_enable)
                    beep_enable = false;
                break;
        }
        
        time_count ++;

        vTaskDelay(100);
    }
}

