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
    char buffer_cache[20] = "test\n";
    queue_record_data_t record_data = {0};

    record_data.size = strlen(buffer_cache);
    record_data.record_buffer = (uint8_t *)buffer_cache;

    thread_pvparam->key_event = xEventGroupCreate();
    led_init();
    beep_init();
    key_init(thread_pvparam->key_event);

    do{
        vTaskDelay(10);
        ESP_LOGI(__FUNCTION__, "wait record_queue create ..");
    }while(!thread_pvparam->record_queue);

    ESP_LOGI(__FUNCTION__, "thread start!");
    while(1)
    {
        if (beep_enable) 
        {
            beep_trigger(1);
            beep_set_freq(freq_count);

            sprintf(buffer_cache, "test %d", freq_count);
            xQueueSend(thread_pvparam->record_queue,
                       &record_data,
                       10);
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

