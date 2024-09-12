#ifndef __THREAD_INIT_H__
#define __THREAD_INIT_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#define PORT_USART_NUM 8

typedef struct {
    float temp_value;
    float humi_value;
} queue_temp_humi_t;

typedef struct {
    uint32_t pressure_sum_value;
    uint16_t pressure_max_value;
    uint16_t pressure_min_value;
} single_pressure_context_t;

typedef struct {
    uint8_t port_connect_flag;
    single_pressure_context_t pressure_context[PORT_USART_NUM];
    uint32_t com_bsp;
} queue_pressure_t;

#define thread_malloc pvPortMalloc
#define thread_free vPortFree
#define thread_realloc pvPortRealloc

typedef struct {
    QueueHandle_t queue_temp_humi;
    QueueHandle_t queue_pressure;
    QueueHandle_t record_queue;
    EventGroupHandle_t key_event;
    uint16_t      tags;
} thread_pvparam_t;

void thraed_work_init(void);

#endif
