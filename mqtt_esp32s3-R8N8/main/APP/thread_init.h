#ifndef __THREAD_INIT_H__
#define __THREAD_INIT_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

//#define ENABLE_LCD

typedef struct {
    float temp_value;
    float humi_value;
} queue_temp_humi_t;

typedef struct {
    uint32_t pressure_sum_value;
    uint16_t pressure_max_value;
    uint16_t pressure_min_value;
} queue_pressure_t;


typedef struct {
    QueueHandle_t queue_temp_humi;
    QueueHandle_t queue_pressure;
    uint16_t      tags;
} thread_pvparam_t;

void thraed_work_init(void);

#endif
