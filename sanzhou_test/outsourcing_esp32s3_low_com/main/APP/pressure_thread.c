#include "thread_init.h"
#include "pressure_thread.h"
#include "usart.h"
#include "rs2251_usart.h"

#define UART_CACHE_BUFFER_SIZE 64
//#define DEBUG_UART_DATA

typedef struct {
    uint8_t *buffer;
    uint16_t buffer_size;

    uint16_t buffer_frame_start_pos;
    uint16_t buffer_frame_end_pos;

    uint8_t *buffer_rw_ptr;
    uint8_t *buffer_end_ptr;

} parser_buffer_context_t;

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

int init_parser_context_struct(parser_buffer_context_t *parser_buffer_context, size_t cache_size)
{
    uint8_t *cache_buffer = NULL;

    cache_buffer = pvPortMalloc(cache_size);

    if (cache_buffer)
    {
        parser_buffer_context->buffer = cache_buffer;
        parser_buffer_context->buffer_size = cache_size;
        parser_buffer_context->buffer_frame_start_pos = 0x00;
        parser_buffer_context->buffer_frame_end_pos = 0x00;
        parser_buffer_context->buffer_rw_ptr = cache_buffer;
        parser_buffer_context->buffer_end_ptr = cache_buffer + cache_size;
        return 0;
    } else {
        parser_buffer_context->buffer = NULL;
        parser_buffer_context->buffer_size = 0;
        parser_buffer_context->buffer_frame_start_pos = 0x00;
        parser_buffer_context->buffer_frame_end_pos = 0x00;
        parser_buffer_context->buffer_rw_ptr = NULL;
        parser_buffer_context->buffer_end_ptr = NULL;
        return -1;
    }

}

int reset_parser_context_struct(parser_buffer_context_t *parser_buffer_context)
{
    parser_buffer_context->buffer_frame_start_pos = 0x00;
    parser_buffer_context->buffer_frame_end_pos = 0x00;
    parser_buffer_context->buffer_rw_ptr = parser_buffer_context->buffer;
    return 0;
}

int fill_uart_data_for_parser_buffer(uart_port_t uart_port,
                                     parser_buffer_context_t *input_buffer_context)
{
    size_t buffer_cached_size = 0x00;
    int ret = 0x0;
    //uart_get_buffered_data_len(uart_port, &buffer_cached_size)
    //if(buffer_cached_size >= input_buffer_context->buffer_size)
    {
        ret = uart_read_bytes(uart_port, input_buffer_context->buffer, input_buffer_context->buffer_size, 20);
        if (ret > 0)
        {
            input_buffer_context->buffer_rw_ptr = input_buffer_context->buffer;
            input_buffer_context->buffer_end_ptr = input_buffer_context->buffer + ret;
            return 0;
        } else {
            return -1;
        }
    }
    //else {
    //    return -1;
    //}
}


int find_target_data_form_buffer(uart_port_t uart_port,
                                 parser_buffer_context_t *input_buffer_context,
                                 parser_buffer_context_t *output_buffer_context)
{
    uint8_t parser_count = 0;
    uint8_t buffer_frame_size = 0;
    int ret = 0;

fill_cache_buffer:

    do {
        ret = fill_uart_data_for_parser_buffer(uart_port, input_buffer_context);
    } while(ret < 0);

    do {
        if (input_buffer_context->buffer_rw_ptr < input_buffer_context->buffer_end_ptr)
        {
            if (*(input_buffer_context->buffer_rw_ptr) == 0x0d)
            {
                if (input_buffer_context->buffer_rw_ptr + 1 < input_buffer_context->buffer_end_ptr)
                {
                    if (*(input_buffer_context->buffer_rw_ptr + 1) == 0x0a)
                    {
                        output_buffer_context->buffer_rw_ptr = output_buffer_context->buffer;
                        input_buffer_context->buffer_rw_ptr += 2;
                        ESP_LOGI("-", "got one frame");
                    }
                } else {
                    ESP_LOGI(__FUNCTION__, "fill buffer once");
                    goto fill_cache_buffer;
                }
            }
            else
            {
                *(output_buffer_context->buffer_rw_ptr) = *(input_buffer_context->buffer_rw_ptr);
                input_buffer_context->buffer_rw_ptr ++;
                output_buffer_context->buffer_rw_ptr ++;
            }
        } else {
            ESP_LOGI(__FUNCTION__, "fill buffer once");
            goto fill_cache_buffer;
        }

    } while(1);

    return 0;

}

int find_uart_data_bits(uart_port_t uart_port, uint8_t target_bits, uint8_t *read_char)
{
    int ret = 0x00;
    ret = uart_read_bytes(uart_port, read_char, 1, 20);
    if (ret < 1)
        return -1;
    else if (*read_char != target_bits)
        return 1;
    else
        return 0;
}

int pressure_update_form_uart_pos(uint8_t *cache_buffer,
        size_t buffer_size,
        pressure_device_info_t *pressure_device_info,
        uart_port_t uart_port)
{
    uint8_t frame_size_count = 0x00;
    uint8_t real_read_char = 0x00;
    int ret = 0;

    if (cache_buffer != NULL)
        memset(cache_buffer, 0x00, buffer_size);

    do {
        ret = find_uart_data_bits(uart_port, 0x0d, &real_read_char);
        if(ret == 0)
        {
            ret = find_uart_data_bits(uart_port, 0x0a, &real_read_char);
            if(ret == 0)
                break;
            else if (ret == 1) {
                cache_buffer[frame_size_count ++] = real_read_char;
            }
        } else {
            if (frame_size_count > buffer_size)
            {
                ESP_LOGE(__FUNCTION__, "rx_buffered_len error");
                break;
            }
            if (ret == 1)
                cache_buffer[frame_size_count ++] = real_read_char;
        }

    } while(1);


    if (frame_size_count <= buffer_size)
    {
        sscanf((char *)cache_buffer, "%d,%f,%f,%f,%f,%f", &pressure_device_info->test_point_num,
                &pressure_device_info->pos_x,
                &pressure_device_info->pos_y,
                &pressure_device_info->pos_z,
                &pressure_device_info->pressure,
                &pressure_device_info->resi);
        //ESP_LOGI(__FUNCTION__, "%s", cache_buffer);
    }
    //ESP_LOGI(__FUNCTION__, "got one frame! %s xyz, %u,%f,%f",cache_buffer, pos_x, pos_y, pos_z);
    return frame_size_count;
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
        ret = uart_read_bytes(uart_port, cmd_buffer, 1, 20);
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

#define PRESS_COM_BPS 460800

void pressure_thread(void *pvparams)
{
    uint8_t *uart1_rx_buffer = pvPortMalloc(UART_CACHE_BUFFER_SIZE);
    parser_buffer_context_t cache_parser_buffer_context = {0};
    parser_buffer_context_t single_frame_parser_buffer_context = {0};
    uint64_t timer_count = 0;
    thread_pvparam_t *thread_pvparam = (thread_pvparam_t *)pvparams;
    queue_pressure_t queue_pressure = {0};
    int rt_value = 0x00;
    uint8_t port_count = 0;
    uint8_t count = 0;
    size_t rx_buffer_size = 0x00;
    EventBits_t pressure_raw_upload_event_bit = 0x00;
    QueueHandle_t uart0_queue;
    uart_event_t uart0_event;

    if (init_parser_context_struct(&cache_parser_buffer_context, UART_CACHE_BUFFER_SIZE * 10) == 0)
    {
        ESP_LOGI(__FUNCTION__, "init cache_parser_buffer_context success");
    } else{
        ESP_LOGE(__FUNCTION__, "init cache_parser_buffer_context failed");
    }

    if (init_parser_context_struct(&single_frame_parser_buffer_context, UART_CACHE_BUFFER_SIZE) == 0)
    {
        ESP_LOGI(__FUNCTION__, "init single_frame_parser_buffer_context success");
    } else {
        ESP_LOGE(__FUNCTION__, "init single_frame_parser_buffer_context failed");
    }

    thread_pvparam->queue_pressure = xQueueCreate(1, sizeof(queue_pressure_t));

    thread_pvparam->pressure_raw_upload_event = xEventGroupCreate();

    usart_init(PRESS_COM_BPS, SENSOR_INPUT_PORT, GPIO_NUM_6, GPIO_NUM_7, NULL);
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

        queue_pressure.com_bsp = PRESS_COM_BPS;

        if (1)
        {
            pressure_update_form_uart_pos(uart1_rx_buffer,
                                          UART_CACHE_BUFFER_SIZE,
                                          &queue_pressure.pressure_device_info,
                                          SENSOR_INPUT_PORT);

            ESP_LOGI("-", "%0.1f", queue_pressure.pressure_device_info.pos_z);

            queue_pressure.port_frame_count ++;
            //xQueueSend(thread_pvparam->queue_pressure, &queue_pressure, 0);
            //ESP_LOGI(__FUNCTION__, "count %lld", queue_pressure.port_frame_count);
        } else {
            find_target_data_form_buffer(SENSOR_INPUT_PORT,
                                         &cache_parser_buffer_context,
                                         &single_frame_parser_buffer_context);
        }


        if (0)
        {
            if (xQueueReceive(uart0_queue, &uart0_event, 10) == pdPASS)
            {
                uart_read_bytes(SENSOR_INPUT_PORT, uart1_rx_buffer, uart0_event.size, 10);
                //ESP_LOGI(__FUNCTION__, "%s types %d size %d", usart1_rx_buffer, uart0_event.type, uart0_event.size);
                memset(uart1_rx_buffer, 0x00, uart0_event.size);
            }
        }
    }
}
