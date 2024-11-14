#include "esp_wifi.h"
#include "esp_event.h"
#include "lwip/apps/mqtt.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "mqtt_client.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_netif.h"
#include "lcd.h"
#include "cJSON.h"
#include "thread_init.h"
#include "lwip_thread.h"
#include "wifi_config.h"
#include "tip_thread.h"



int g_publish_flag = 0;/* 发布成功标志位 */
static const char *TAG = "MQTT_EXAMPLE";
char g_lcd_buff[100] = {0};

typedef struct{
    char test_char[32];
    EventGroupHandle_t pressure_raw_upload_event;
    EventGroupHandle_t tip_event;
    QueueHandle_t queue_pressure;
    uint8_t *cache_pressure_raw_buffer;
    uint16_t cache_pressure_raw_buffer_size;
} mqtt_handle_arg_t;

/**
 * @brief       错误日记
 * @param       message     :错误消息
 * @param       error_code  :错误码
 * @retval      无
 */
static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

void mqtt_cmd_parser(EventGroupHandle_t event_handle, char *mqtt_cmd, size_t mqtt_cmd_size)
{
    cJSON *cjson_handle = NULL;
    char *json_value = NULL;
    uint8_t cmd_lens = 0;
    uint8_t com_num = 0;

    cjson_handle = cJSON_Parse(mqtt_cmd);
    json_value = cJSON_GetStringValue(cJSON_GetObjectItem(cjson_handle, "action"));
    if (json_value)
    {
        cmd_lens = MIN(strlen(json_value), strlen(MQTT_DEVICE_CMD_RETURN_RAW_VALUE));
        if (!strncmp(json_value, MQTT_DEVICE_CMD_RETURN_RAW_VALUE, cmd_lens))
        {
            ESP_LOGI(__FUNCTION__, "match cmd \"%s\" success", json_value);
        }
        json_value = NULL;
    }

    json_value = cJSON_GetStringValue(cJSON_GetObjectItem(cjson_handle, "com"));
    if (json_value)
    {
        com_num = json_value[0] - '0';
        if (com_num < 8)
        {
            ESP_LOGI(__FUNCTION__, "match cmd \"%d\" success", com_num);
            xEventGroupSetBits(event_handle, EVENT_GET_PRESSURE_RAW_COM_0 << com_num);
        }

        json_value = NULL;
    }

    if (cjson_handle)
        cJSON_Delete(cjson_handle);
}

/**
 * @brief       注册接收MQTT事件的事件处理程序
 * @param       handler_args:注册到事件的用户数据
 * @param       base        :处理程序的事件库
 * @param       event_id    :接收到的事件的id
 * @param       event_data  :事件的数据
 * @retval      无
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    mqtt_handle_arg_t *mqtt_handle_arg = (mqtt_handle_arg_t *)handler_args;
    queue_pressure_t queue_pressure = {0};

    switch ((esp_mqtt_event_id_t)event_id)
    {
        case MQTT_EVENT_CONNECTED:      /* 连接事件 */
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            g_publish_flag = 1;
            /* 订阅主题 */
            msg_id = esp_mqtt_client_subscribe(client, DEVICE_SUBSCRIBE, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, DEVICE_CMD_RECEIVE_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            break;
        case MQTT_EVENT_DISCONNECTED:   /* 断开连接事件 */
            ESP_LOGE(TAG, "mqtt disconnect! check wifi connect!!!");
            break;

        case MQTT_EVENT_SUBSCRIBED:     /* 取消事件 */
            break;
        case MQTT_EVENT_UNSUBSCRIBED:   /* 取消订阅事件 */
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:      /* 发布事件 */
            ESP_LOGD(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:           /* 接收数据事件 */
            ESP_LOGD(__FUNCTION__, "TOPIC = %.*s\r\n", event->topic_len, event->topic);
            ESP_LOGD(__FUNCTION__, "DATA = %.*s\r\n", event->data_len, event->data);

            uint16_t topic_len = event->topic_len < strlen(DEVICE_CMD_RECEIVE_TOPIC) ? event->topic_len : strlen(DEVICE_CMD_RECEIVE_TOPIC);
            if (!strncmp(event->topic, DEVICE_CMD_RECEIVE_TOPIC, topic_len))
            {
                mqtt_cmd_parser(mqtt_handle_arg->pressure_raw_upload_event, event->data, event->data_len);
                ESP_LOGI(__FUNCTION__, "TOPIC = %.*s\r\n", event->topic_len, event->topic);
            }

            if (!strncmp(event->topic, DEVICE_BINARY_RECEIVE_TOPIC, topic_len))
            {
                ESP_LOGI(__FUNCTION__, "topic %.*s DATA size = %d\r\n",event->topic_len, event->topic, event->data_len);
                memset(mqtt_handle_arg->cache_pressure_raw_buffer, 0x00, mqtt_handle_arg->cache_pressure_raw_buffer_size);
                memcpy(mqtt_handle_arg->cache_pressure_raw_buffer, event->data, event->data_len);
                queue_pressure.raw_data = mqtt_handle_arg->cache_pressure_raw_buffer;
                queue_pressure.raw_data_size = event->data_len;
                queue_pressure.total_buffer_size = mqtt_handle_arg->cache_pressure_raw_buffer_size;
                xQueueOverwrite(mqtt_handle_arg->queue_pressure, &queue_pressure);
                xEventGroupSetBits(mqtt_handle_arg->tip_event, TIP_EVENT_BIT_2);
            }

            break;
        case MQTT_EVENT_ERROR:
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
            {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }

            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

/**
 * @brief       lwip_demo进程
 * @param       无
 * @retval      无
 */
void lwip_thread_demo(void)
{

}

static void lwip_init(esp_mqtt_client_handle_t *mqtt_client_handle, cJSON *json_handle, void *mqtt_handle_arg)
{
    /* 设置客户端的信息量 */
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.hostname = HOST_NAME,                   /* MQTT地址 */
        .broker.address.port = HOST_PORT,                       /* MQTT端口号 */
        .broker.address.transport = MQTT_TRANSPORT_OVER_TCP,    /* TCP模式 */
        .credentials.client_id = CLIENT_ID,                     /* 设备名称 */
        .credentials.username = (char *)USER_NAME,               /* 产品ID */
        .credentials.authentication.password = PASSWORD,        /* 计算出来的密码 */
    };

    *mqtt_client_handle = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(*mqtt_client_handle, ESP_EVENT_ANY_ID, mqtt_event_handler, mqtt_handle_arg);
    esp_mqtt_client_start(*mqtt_client_handle);
}

static void lwip_deinit(esp_mqtt_client_handle_t *mqtt_client_handle)
{
    esp_mqtt_client_stop(*mqtt_client_handle);
}

void lwip_send_demo(esp_mqtt_client_handle_t *mqtt_client_handle)
{
    char mqtt_publish_data[] =
        "{ \"params\":{\
        \"test_bufffer_send\":\"test_press_num = 123456\",\
        \"Temperature\":80,\
        \"OutputVoltage\":6\
}\
    }";

esp_mqtt_client_publish(*mqtt_client_handle, DEVICE_SENSOR_INFO_POS, mqtt_publish_data, strlen(mqtt_publish_data), 1, 0);
}

#define PRESSURE_COM_DEF "Pressure_com"
#define RAW_CACHE_PRESSURE_DATA_SIZE 2560

void lwip_thread(void *pvparams)
{
    esp_mqtt_client_handle_t mqtt_client_handle;
    thread_pvparam_t *thread_pvparam = (thread_pvparam_t *)pvparams;
    queue_temp_humi_t queue_temp_humi = {0};
    queue_json_cmd_t queue_json_cmd = {0};
    EventGroupHandle_t wifi_event;
    cJSON *mqtt_json_base = NULL, *mqtt_json_param = NULL;
    char *mqtt_buffer = NULL;
    char cache_buffer[32] = {0x00};
    uint8_t cache_count = 0;
    uint64_t timer_count = 0;
    uint32_t display_color = 0;
#ifdef ENABLE_LCD
    char lcd_buffer_display[64] = {0};
#endif
    bool temp_flag = false;
    uint8_t upload_flag = 0x00;
    EventBits_t wifi_connect_bits;
    mqtt_handle_arg_t mqtt_handle_arg = {"mqtt event", NULL, NULL};
    EventBits_t pressure_raw_upload_event_bit = 0x00;
    uint8_t *cache_pressure_raw_buffer = pvPortMalloc(RAW_CACHE_PRESSURE_DATA_SIZE);


    wifi_sta_init();

    while(thread_pvparam->queue_temp_humi == NULL) vTaskDelay(10);
    while(thread_pvparam->queue_pressure == NULL) vTaskDelay(10);
    while(thread_pvparam->queue_json_cmd == NULL) vTaskDelay(10);
    while(thread_pvparam->tip_event == NULL) vTaskDelay(10);
    while(thread_pvparam->pressure_raw_upload_event == NULL) vTaskDelay(10);

    mqtt_handle_arg.pressure_raw_upload_event = thread_pvparam->pressure_raw_upload_event;
    mqtt_handle_arg.queue_pressure = thread_pvparam->queue_pressure;
    mqtt_handle_arg.cache_pressure_raw_buffer = cache_pressure_raw_buffer;
    mqtt_handle_arg.cache_pressure_raw_buffer_size = RAW_CACHE_PRESSURE_DATA_SIZE;
    mqtt_handle_arg.tip_event = thread_pvparam->tip_event;

    lwip_init(&mqtt_client_handle, mqtt_json_base, (void *)&mqtt_handle_arg);
    wifi_event = get_wifi_event_handle();

    mqtt_json_base = cJSON_CreateObject();
    mqtt_json_param = cJSON_CreateObject();

    cJSON_AddItemToObject(mqtt_json_base, "params", mqtt_json_param);
    cJSON_AddItemToObject(mqtt_json_param, "Temperature", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(mqtt_json_param, "Humidity", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(mqtt_json_param, "illumination", cJSON_CreateNumber(0));


    //cJSON_AddItemToObject(mqtt_json_param, "test_bufffer_send", cJSON_CreateRaw(cache_buffer));

    for(cache_count = 0; cache_count < PORT_USART_NUM; cache_count ++)
    {
        sprintf(cache_buffer, "%s%u", PRESSURE_COM_DEF, cache_count);
        cJSON_AddItemToObject(mqtt_json_param, cache_buffer, cJSON_CreateNumber(0));
        memset(cache_buffer, 0x00, sizeof(cache_buffer));
    }

    while(1)
    {
        if (xQueueReceive(thread_pvparam->queue_temp_humi, &queue_temp_humi, 0) == pdPASS)
        {
            cJSON_ReplaceItemInObject(mqtt_json_param, "Temperature", cJSON_CreateNumber(queue_temp_humi.temp_value));
            cJSON_ReplaceItemInObject(mqtt_json_param, "Humidity", cJSON_CreateNumber(queue_temp_humi.humi_value));
            temp_flag = true;
            upload_flag |= 0x01;
        }

        if (xQueueReceive(thread_pvparam->queue_json_cmd, &queue_json_cmd, 0) == pdPASS)
        {
            ESP_LOGI(__FUNCTION__, "%.*s", queue_json_cmd.json_cmd_buffer_size, queue_json_cmd.json_cmd_buffer);
            xEventGroupSetBits(thread_pvparam->tip_event, TIP_EVENT_BIT_1);
            esp_mqtt_client_publish(mqtt_client_handle,
                                    DEVICE_CMD_SEND_TOPIC,
                                    (char *)queue_json_cmd.json_cmd_buffer,
                                    queue_json_cmd.json_cmd_buffer_size,
                                    1,
                                    0);
        }


        wifi_connect_bits = xEventGroupWaitBits(wifi_event,
                                                WIFI_CONNECTED_BIT | WIFI_FAIL_BIT | WIFI_CONNECTING_BIT,
                                                pdTRUE,
                                                pdFALSE,
                                                0);
        if (wifi_connect_bits & WIFI_FAIL_BIT)
        {
            xEventGroupSetBits(thread_pvparam->tip_event, TIP_EVENT_BIT_4);
        }

        if (wifi_connect_bits & WIFI_CONNECTING_BIT)
        {
            xEventGroupSetBits(thread_pvparam->tip_event, TIP_EVENT_BIT_0);
        }

        if (wifi_connect_bits & WIFI_CONNECTED_BIT)
        {
            xEventGroupSetBits(thread_pvparam->tip_event, TIP_EVENT_BIT_4);
        }

#ifdef ENABLE_LCD

        if (wifi_connect_bits & WIFI_FAIL_BIT)
        {
            xEventGroupSetBits(thread_pvparam->tip_event, TIP_EVENT_BIT_0);
            lcd_show_string(50, 40, 240, 16, 16, "wifi connect failed !", RED);
        }

        if (wifi_connect_bits & WIFI_CONNECTING_BIT)
        {
            xEventGroupSetBits(thread_pvparam->tip_event, TIP_EVENT_BIT_0);
            lcd_show_string(50, 40, 240, 16, 16, "wifi reconnecting ....", RED);
        }

        if (wifi_connect_bits & WIFI_CONNECTED_BIT)
        {
            xEventGroupClearBits(thread_pvparam->tip_event, TIP_EVENT_BIT_0);
            lcd_show_string(50, 40, 240, 16, 16, "wifi connect success !", BLUE);
        }
    uint8_t port_connect_flag;

        sprintf(lcd_buffer_display, "MQTT %s&%s", PRODUCT_KEY, DEVICE_NAME);
        lcd_show_string(50, 80, 240, 16, 16, lcd_buffer_display, RED);

        if (temp_flag)
            sprintf(lcd_buffer_display, "temp %0.1f humi %0.1f",
                                    queue_temp_humi.temp_value,
                                    queue_temp_humi.humi_value);
        else
            sprintf(lcd_buffer_display, "temp sensor err !!!");

        lcd_show_string(50, 100, 240, 24, 24, lcd_buffer_display, RED);

        memset(lcd_buffer_display, 0x00, 64);
        sprintf(lcd_buffer_display, "com bsp %u", 460800);
        lcd_show_string(50, 130, 240, 16, 16, lcd_buffer_display, RED);

        memset(lcd_buffer_display, 0x00, 64);
#endif
#ifdef DEBUG_LWIP_DATA
        ESP_LOGI(__FUNCTION__, "temp %0.1f humi %0.1f Press %ld",
                                queue_temp_humi.temp_value,
                                queue_temp_humi.humi_value,
                                queue_pressure.pressure_context[0].pressure_sum_value);
#endif

        timer_count ++;
        vTaskDelay(5);
    }

    lwip_deinit(&mqtt_client_handle);
}

