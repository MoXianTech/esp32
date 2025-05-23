/**
 ****************************************************************************************************
 * @file        wifi_config.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       网络连接配置
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 ESP32-S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "wifi_config.h"

#define WIFI_MOXIAN_1234

#ifdef WIFI_OUTSIDE_AP
/* 链接wifi名称 */
#define DEFAULT_SSID        "moxian_ceshi"
/* wifi密码 */
#define DEFAULT_PWD         "weizihan"

#endif

#ifdef WIFI_TEST_HOST
/* 链接wifi名称 */
#define DEFAULT_SSID        "test_wifi_host"
/* wifi密码 */
#define DEFAULT_PWD         "aaaBB@@13."
#endif

#ifdef WIFI_TAKE_OFF
/* 链接wifi名称 */
#define DEFAULT_SSID        "takeoff"
/* wifi密码 */
#define DEFAULT_PWD         "weizihan"
#endif

#ifdef WIFI_MATRIX_104
/* 链接wifi名称 */
#define DEFAULT_SSID        "Matrix-104"
/* wifi密码 */
#define DEFAULT_PWD         "12345678"
#endif

#ifdef WIFI_MOXIAN_1234
/* 链接wifi名称 */
#define DEFAULT_SSID        "moxian123"
/* wifi密码 */
#define DEFAULT_PWD         "12345678"
#endif


EventGroupHandle_t wifi_event;

network_connet_info network_connet;
static const char *TAG = "static_ip";
char lcd_buff[100] = {0};

/* WIFI默认配置 */
#define WIFICONFIG()   {                            \
    .sta = {                                        \
        .ssid = DEFAULT_SSID,                       \
        .password = DEFAULT_PWD,                    \
        .threshold.authmode = WIFI_AUTH_WPA2_PSK,   \
    },                                              \
}

/**
 * @brief       链接显示
 * @param       flag:2->链接;1->链接失败;0->再链接中
 * @retval      无
 */
bool enable_wifi_display = 1;
void connet_display(uint8_t flag)
{
    if (enable_wifi_display)
    {
#ifdef ENABLE_LCD
        if((flag & 0x80) == 0x80)
        {
            lcd_fill(0,0,320,240,WHITE);
            sprintf(lcd_buff, "ssid:%s",DEFAULT_SSID);
            lcd_show_string(50, 0, 240, 16, 16, lcd_buff, BLUE);
            sprintf(lcd_buff, "psw:%s",DEFAULT_PWD);
            lcd_show_string(50, 20, 240, 16, 16, lcd_buff, BLUE);
        }
        else if ((flag & 0x04) == 0x04)
        {
            lcd_show_string(50, 40, 240, 16, 16, "wifi connecting......", BLUE);
        }
        else if ((flag & 0x02) == 0x02)
        {
            lcd_show_string(50, 40, 240, 16, 16, "wifi connecting fail", BLUE);
            sprintf(lcd_buff, "wifi connect err !!!!!!!!!");
            lcd_show_string(50, 0, 240, 16, 16, lcd_buff, BLUE);
            lcd_show_string(50, 20, 240, 16, 16, lcd_buff, BLUE);
        }
        else if ((flag & 0x01) == 0x01)
        {
            printf("%s\r\n",network_connet.ip_buf);
            lcd_show_string(50, 40, 240, 16, 16, "wifi connect success !", BLUE);
            lcd_show_string(50, 60, 200, 16, 16, (char*)network_connet.ip_buf, MAGENTA);
        }
#endif
    }
    network_connet.connet_state &= 0x00;
}

/**
 * @brief       WIFI链接糊掉函数
 * @param       arg:传入网卡控制块
 * @param       event_base:WIFI事件
 * @param       event_id:事件ID
 * @param       event_data:事件数据
 * @retval      无
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static int s_retry_num = 0;
    /* 扫描到要连接的WIFI事件 */
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        network_connet.connet_state |= 0x04;
        network_connet.fun(network_connet.connet_state);
        esp_wifi_connect();
    }
    /* 连接WIFI事件 */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        network_connet.connet_state |= 0x80;
        network_connet.fun(network_connet.connet_state);
    }
    /* 连接WIFI失败事件 */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        network_connet.connet_state |= 0x02;
        /* 尝试连接 */
            ESP_LOGI(__FUNCTION__, "TIP_EVENT_BIT_0");
        if (s_retry_num < 50)
        {
            esp_wifi_connect();
            s_retry_num ++;
            xEventGroupSetBits(wifi_event, WIFI_CONNECTING_BIT);
            network_connet.fun(network_connet.connet_state);
            ESP_LOGI(TAG, "retry to connect to the AP %d", s_retry_num);
        }
        else
        {
            xEventGroupSetBits(wifi_event, WIFI_FAIL_BIT);
            network_connet.fun(network_connet.connet_state);
        }

        ESP_LOGI(TAG,"connect to the AP fail");
    }
    /* 工作站从连接的AP获得IP */
    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        network_connet.connet_state |= 0x01;
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "static ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        sprintf(network_connet.ip_buf, "static ip:" IPSTR, IP2STR(&event->ip_info.ip));
        network_connet.fun(network_connet.connet_state);
        xEventGroupSetBits(wifi_event, WIFI_CONNECTED_BIT);
    }
}

EventGroupHandle_t get_wifi_event_handle()
{
    return wifi_event;
}

/**
 * @brief       WIFI初始化
 * @param       无
 * @retval      无
 */
void wifi_sta_init()
{
    static esp_netif_t *sta_netif = NULL;
    network_connet.connet_state = 0x00;
    network_connet.fun = connet_display;
    EventBits_t bits = 0;

    wifi_event = xEventGroupCreate();    /* 创建一个事件标志组 */
    /* 网卡初始化 */
    ESP_ERROR_CHECK(esp_netif_init());
    /* 创建新的事件循环 */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    sta_netif= esp_netif_create_default_wifi_sta();
    assert(sta_netif);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL) );
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t  wifi_config = WIFICONFIG();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start());

    /* 等待链接成功后、ip生成 */
    do{
        bits = xEventGroupWaitBits( wifi_event,
                                    WIFI_FAIL_BIT | WIFI_CONNECTED_BIT | WIFI_CONNECTING_BIT,
                                    pdFALSE,
                                    pdFALSE,
                                    100);
        if (bits & WIFI_FAIL_BIT)
        {
            ESP_LOGE(TAG, "Failed to connect to SSID:%s, password:%s",
                    DEFAULT_SSID, DEFAULT_PWD);
            vTaskDelay(300);
        }
        if (bits & WIFI_CONNECTED_BIT)
        {
            ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                    DEFAULT_SSID, DEFAULT_PWD);
            enable_wifi_display = 0;
            break;
        }

        xEventGroupClearBits(wifi_event, WIFI_FAIL_BIT | WIFI_CONNECTED_BIT | WIFI_CONNECTING_BIT);
    } while(1);

}
