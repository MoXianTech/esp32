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
    int test_point_num;
    float pos_x;
    float pos_y;
    float pos_z;
    float pressure;
    float resi;
} pressure_device_info_t;

typedef struct {
    uint8_t port_connect_flag;
    uint64_t port_frame_count;
    uint32_t com_bsp;
    pressure_device_info_t pressure_device_info;
} queue_pressure_t;

#define thread_malloc pvPortMalloc
#define thread_free vPortFree
#define thread_realloc pvPortRealloc

typedef struct {
    QueueHandle_t queue_temp_humi;
    QueueHandle_t queue_pressure;
    QueueHandle_t record_queue;
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
