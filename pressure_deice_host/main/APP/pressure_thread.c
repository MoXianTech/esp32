#include "thread_init.h"
#include "pressure_thread.h"
#include "usart.h"
#include "rs2251_usart.h"

#define UART_CACHE_BUFFER_SIZE 4096
//#define DEBUG_UART_DATA

uint16_t cal_check_sum(uint8_t *data, uint16_t len)
{
    uint16_t sum = 0;
    for(int i = 0; i < len; ++i)
    {
        sum += data[i];
    }
    return sum;
}

int cmd_buffer_check(uint8_t *cmd_buffer, size_t len)
{
    uint16_t checksum_value = cmd_buffer[len - 2] | (cmd_buffer[len - 1] << 8);
    if (checksum_value != cal_check_sum(cmd_buffer, len - 2))
    {
        ESP_LOGE(__FUNCTION__, "checksum err! checksum_value %d != %d",
                checksum_value,
                cal_check_sum(cmd_buffer, len - 2));
        return -1;
    }
    else {
        return 0;
    }
}

#define CMD_HEADER_LEN 6

uint32_t get_pressure_sum_value(uint8_t *cmd_buffer, size_t len)
{
    uint32_t sum_value = 0;
    sum_value = cmd_buffer[len - 2] | (cmd_buffer[len - 1] << 8);
    for(int i = 0; i < CMD_HEADER_LEN; ++i)
    {
        sum_value -= cmd_buffer[i];
    }
    return sum_value;
}

int try_parser_header_form_uart(uart_port_t uart_port)
{
    uint8_t cmd_header_flag = 0x00;
    uint8_t cmd_buffer[1] = {0};
    do {
        uart_read_bytes(uart_port, cmd_buffer, 1, 100);
        if (cmd_buffer[0] == 0xA5)
        {
            cmd_header_flag |= 0x01;
            break;
        }
    } while(1);

    return 0;
}


int pressure_parser_form_uart(uint8_t *cmd_buffer,
        size_t buffer_size,
        size_t *cmd_size,
        uart_port_t uart_port)
{
    uint8_t cmd_header_flag = 0x00;
    size_t rx_buffer_size = 0x00, real_rx_size = 0;
    int ret = 0;
    *cmd_size = 0x00;

rework_port_read:
    memset(cmd_buffer, 0x00, CMD_HEADER_LEN);
    rx_buffer_size = 0x00;
    real_rx_size = 0x00;
    cmd_header_flag = 0x00;

    do {
        ret = uart_read_bytes(uart_port, cmd_buffer, 1, 50);
        if (ret < 1)
            return -1;
        if (cmd_buffer[0] == 0xA5)
        {
            cmd_header_flag |= 0x01;
            break;
        }
    } while(1);

    uart_read_bytes(uart_port, cmd_buffer + 1, 1, 100);
    if (cmd_buffer[1] == 0x5A)
    {
        cmd_header_flag |= 0x02;
    }

    if (cmd_header_flag != 0x03)
        goto rework_port_read;

    uart_read_bytes(uart_port, cmd_buffer + 2, 4, 300);
    rx_buffer_size = (cmd_buffer[3] | (cmd_buffer[4] << 8)) + 2;
    if (rx_buffer_size > buffer_size)
    {
        ESP_LOGE(__FUNCTION__, "receive CMD size %d cache buffer size %d", rx_buffer_size, buffer_size);
        return -1;
    }

    real_rx_size = uart_read_bytes(uart_port, cmd_buffer + CMD_HEADER_LEN, rx_buffer_size - CMD_HEADER_LEN, 300);
    if (((real_rx_size += CMD_HEADER_LEN) != (rx_buffer_size)) || (cmd_buffer_check(cmd_buffer, rx_buffer_size) != 0))
    {
        ESP_LOGE(__FUNCTION__, "CMD recive ERR real_rx_size %d != rx_buffer_size %d", real_rx_size, rx_buffer_size);
        goto rework_port_read;
    }
    else
    {
        *cmd_size = rx_buffer_size;
    }
    return rx_buffer_size;
}

#define SENSOR_INPUT_PORT UART_NUM_1
RS2251_USART_NUM_T rs2251_usart_range[PORT_USART_NUM] = {
    USART_NUM_0,
    USART_NUM_1,
    USART_NUM_2,
    USART_NUM_3,
    USART_NUM_4,
    USART_NUM_5,
    USART_NUM_6,
    USART_NUM_7
};

uint16_t CalChecksum(uint8_t *data, uint16_t len)
{
    uint16_t sum = 0;
    for(int i = 0; i < len; ++i)
    {
        sum += data[i];
    }
    return sum;
}

#define PRESS_COM_BPS 460800

void pressure_thread(void *pvparams)
{
    uint8_t *usart1_rx_buffer = pvPortMalloc(UART_CACHE_BUFFER_SIZE);
    uint8_t *json_cmd_buffer = pvPortMalloc(UART_CACHE_BUFFER_SIZE);
    uint8_t *cache_tx_buffer = pvPortMalloc(UART_CACHE_BUFFER_SIZE);
    uint16_t cache_tx_buffer_size = UART_CACHE_BUFFER_SIZE;
    uint16_t cache_tx_data_size = 8;
    uint16_t check_sum_value = 0;

    memset(cache_tx_buffer, 0x00, UART_CACHE_BUFFER_SIZE);
    thread_pvparam_t *thread_pvparam = (thread_pvparam_t *)pvparams;
    queue_pressure_t queue_pressure = {0};
    queue_json_cmd_t queue_json_cmd = {0};

    int rt_value = 0x00;
    uint8_t port_count = 0;
    size_t rx_buffer_size = 0x00;
    EventBits_t pressure_raw_upload_event_bit = 0x00;
    size_t cache_size = 0x00, last_cache_size = 0x00;
    uint64_t thread_time_count = 0;

    thread_pvparam->queue_pressure = xQueueCreate(1, sizeof(queue_pressure_t));
    thread_pvparam->queue_json_cmd = xQueueCreate(1, sizeof(queue_json_cmd_t));
    thread_pvparam->pressure_raw_upload_event = xEventGroupCreate();

    usart_init(PRESS_COM_BPS, SENSOR_INPUT_PORT, GPIO_NUM_6, GPIO_NUM_7);
    rs2251_usart_init();

    rs2251_usart_ctrl_channel(USART_NUM_0, false, false);
    rs2251_usart_ctrl_channel(USART_NUM_1, false, false);
    rs2251_usart_ctrl_channel(USART_NUM_2, false, false);
    rs2251_usart_ctrl_channel(USART_NUM_3, false, false);
    rs2251_usart_ctrl_channel(USART_NUM_4, false, false);
    rs2251_usart_ctrl_channel(USART_NUM_5, false, false);
    rs2251_usart_ctrl_channel(USART_NUM_6, false, false);
    rs2251_usart_ctrl_channel(USART_NUM_7, false, false);

    rs2251_usart_ctrl_channel(USART_NUM_0, true, true);

    while(1)
    {

        if (xQueueReceive(thread_pvparam->queue_pressure, &queue_pressure, 0) == pdPASS)
        {
            ESP_LOGI(__FUNCTION__, "get pressure raw data size %d", queue_pressure.raw_data_size);
            if (queue_pressure.raw_data_size == 2)
            {
                memset(cache_tx_buffer + 6, 0xff, cache_tx_data_size - 8);
                check_sum_value = CalChecksum(cache_tx_buffer, cache_tx_data_size - 2);
                cache_tx_buffer[cache_tx_data_size - 2] = check_sum_value & 0xff;
                cache_tx_buffer[cache_tx_data_size - 1] = (check_sum_value >> 8) & 0xff;
            } else {
                cache_tx_data_size = queue_pressure.raw_data_size;
                memcpy(cache_tx_buffer, queue_pressure.raw_data, cache_tx_data_size);
            }
        }


        if (queue_pressure.raw_data_size && (thread_time_count % 5 == 0))
            uart_write_bytes(SENSOR_INPUT_PORT, cache_tx_buffer, cache_tx_data_size);

        {
            uart_get_buffered_data_len(SENSOR_INPUT_PORT, &cache_size);
            if ((cache_size != 0) && (last_cache_size == cache_size))
            {
                uart_read_bytes(SENSOR_INPUT_PORT, usart1_rx_buffer, cache_size, 10);
                ESP_LOGI(__FUNCTION__, "rx cache buffer size %d", cache_size);
                ESP_LOGI(__FUNCTION__, "%.*s", cache_size, usart1_rx_buffer);
                memcpy(json_cmd_buffer, usart1_rx_buffer, cache_size);
                queue_json_cmd.json_cmd_buffer = json_cmd_buffer;
                queue_json_cmd.json_cmd_buffer_size = cache_size;
                xQueueOverwrite(thread_pvparam->queue_json_cmd, &queue_json_cmd);
            }
            last_cache_size = cache_size;
        }

        if (0)
        {
            uart_flush(SENSOR_INPUT_PORT);
            rt_value = pressure_parser_form_uart(usart1_rx_buffer,
                                                 UART_CACHE_BUFFER_SIZE,
                                                 &rx_buffer_size,
                                                 SENSOR_INPUT_PORT);
            if (rt_value > 0)
            {

            } else {
                ESP_LOGI(__FUNCTION__, "pop pressure %d value err! ret = %d\n", port_count, rt_value);
            }
        }

        thread_time_count ++;
        vTaskDelay(5);
    }
}
