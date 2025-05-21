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
    uint8_t *pressure_buffer;
    uint16_t pressure_buffer_size;
} single_pressure_context_t;

typedef struct {
    uint8_t *raw_data;
    uint16_t raw_data_size;
    uint16_t total_buffer_size;
    uint16_t com_bsp;
} queue_pressure_t;

typedef struct {
    uint8_t *json_cmd_buffer;
    uint16_t json_cmd_buffer_size;
} queue_json_cmd_t;

#define thread_malloc pvPortMalloc
#define thread_free vPortFree
#define thread_realloc pvPortRealloc

typedef struct {
    QueueHandle_t queue_temp_humi;
    QueueHandle_t queue_pressure;
    QueueHandle_t queue_json_cmd;
    EventGroupHandle_t key_event;
    EventGroupHandle_t tip_event;
    EventGroupHandle_t pressure_raw_upload_event;
    uint16_t      tags;
} thread_pvparam_t;

void thraed_work_init(void);

#define EVENT_GET_PRESSURE_RAW_COM_0 BIT0
#define EVENT_GET_PRESSURE_RAW_COM_1 BIT1
#define EVENT_GET_PRESSURE_RAW_COM_2 BIT2
#define EVENT_GET_PRESSURE_RAW_COM_3 BIT3
#define EVENT_GET_PRESSURE_RAW_COM_4 BIT4
#define EVENT_GET_PRESSURE_RAW_COM_5 BIT5
#define EVENT_GET_PRESSURE_RAW_COM_6 BIT6
#define EVENT_GET_PRESSURE_RAW_COM_7 BIT7

#define EVENT_UPLOAD_PRESSURE_RAW_COM_0 BIT8
#define EVENT_UPLOAD_PRESSURE_RAW_COM_1 BIT9
#define EVENT_UPLOAD_PRESSURE_RAW_COM_2 BIT10
#define EVENT_UPLOAD_PRESSURE_RAW_COM_3 BIT11
#define EVENT_UPLOAD_PRESSURE_RAW_COM_4 BIT12
#define EVENT_UPLOAD_PRESSURE_RAW_COM_5 BIT13
#define EVENT_UPLOAD_PRESSURE_RAW_COM_6 BIT14
#define EVENT_UPLOAD_PRESSURE_RAW_COM_7 BIT15

#endif
