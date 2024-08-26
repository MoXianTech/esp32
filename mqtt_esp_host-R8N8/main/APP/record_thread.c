#include "thread_init.h"
#include "record_thread.h"

void record_thread(void *pvparams)
{
    thread_pvparam_t *thread_pvparam = (thread_pvparam_t *)pvparams;
    queue_record_data_t queue_record = {0};
    thread_pvparam->record_queue = xQueueCreate(2, sizeof(queue_record_data_t));

    while(1)
    {
        if(xQueueReceive(thread_pvparam->record_queue, &queue_record, 100) == pdPASS)
        {
            ESP_LOGI(__FUNCTION__, "%s", queue_record.record_buffer);
        }
    }
}
