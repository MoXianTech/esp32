#include "thread_init.h"
#include "pressure_thread.h"
#include "usart.h"

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


int pressure_parser_form_uart(uint8_t *cmd_buffer,
                               size_t buffer_size,
                               size_t *cmd_size,
                               uart_port_t uart_port)
{
    uint8_t cmd_header_flag = 0x00;
    size_t rx_buffer_size = 0x00, real_rx_size = 0;
    *cmd_size = 0x00;

rework_port_read:
    memset(cmd_buffer, 0x00, CMD_HEADER_LEN);
    rx_buffer_size = 0x00;
    real_rx_size = 0x00;
    cmd_header_flag = 0x00;

    do {
        uart_read_bytes(UART_NUM_1, cmd_buffer, 1, 100);
        if (cmd_buffer[0] == 0xA5)
        {
            cmd_header_flag |= 0x01;
            break;
        }
    } while(1);

    uart_read_bytes(UART_NUM_1, cmd_buffer + 1, 1, 100);
    if (cmd_buffer[1] == 0x5A)
    {
        cmd_header_flag |= 0x02;
    }

    if (cmd_header_flag != 0x03)
        goto rework_port_read;

    uart_read_bytes(UART_NUM_1, cmd_buffer + 2, 4, 300);
    rx_buffer_size = (cmd_buffer[3] | (cmd_buffer[4] << 8)) + 2;
    if (rx_buffer_size > buffer_size)
    {
        ESP_LOGE(__FUNCTION__, "receive CMD size %d cache buffer size %d", rx_buffer_size, buffer_size);
        return -1;
    }

    real_rx_size = uart_read_bytes(UART_NUM_1, cmd_buffer + CMD_HEADER_LEN, rx_buffer_size - CMD_HEADER_LEN, 300);
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

void pressure_thread(void *pvparams)
{
    uint8_t *usart1_rx_buffer = pvPortMalloc(UART_CACHE_BUFFER_SIZE);
    thread_pvparam_t *thread_pvparam = (thread_pvparam_t *)pvparams;
    queue_pressure_t queue_pressure = {0};
    int rt_value = 0x00;
    size_t rx_buffer_size = 0x00;

    thread_pvparam->queue_pressure = xQueueCreate(1, sizeof(queue_pressure));
    usart_init(460800, UART_NUM_1, GPIO_NUM_6, GPIO_NUM_7);

    while(1)
    {
        rt_value = pressure_parser_form_uart(usart1_rx_buffer,
                                             UART_CACHE_BUFFER_SIZE,
                                             &rx_buffer_size,
                                             UART_NUM_1);
        if (rt_value > 0)
        {
            queue_pressure.pressure_sum_value = get_pressure_sum_value(usart1_rx_buffer, rx_buffer_size);
            xQueueOverwrite(thread_pvparam->queue_pressure, &queue_pressure);
        }

        vTaskDelay(100);
        uart_flush(UART_NUM_1);

#ifdef DEBUG_UART_DATA
        ESP_LOGI(__FUNCTION__, "rx_buffer_size %d 0x%x 0x%x 0x%x 0x%x", rx_buffer_size,
                                                    usart1_rx_buffer[0],
                                                    usart1_rx_buffer[1],
                                                    usart1_rx_buffer[rx_buffer_size - 1],
                                                    usart1_rx_buffer[rx_buffer_size - 2]
                );
#endif
    }
}
