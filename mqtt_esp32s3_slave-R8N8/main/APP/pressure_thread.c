#include "thread_init.h"
#include "pressure_thread.h"
#include "usart.h"
#include "rs2251_usart.h"

#define UART_CACHE_BUFFER_SIZE 4096
#define DEBUG_UART_DATA

uint16_t cal_check_sum(uint8_t *data, uint16_t len)
{
    uint16_t sum = 0;
    for(int i = 0; i < len; ++i)
    {
        sum += data[i];
    }
    return sum;
}

// ================= CRC 计算函数 =================
/**
 * @brief 计算Modbus协议使用的CRC16校验值（多项式0xA001，初始值0xFFFF）
 * 
 * 算法特点：
 * - 初始值：0xFFFF
 * - 多项式：0xA001（即x^16 + x^15 + x^2 + 1）
 * - 数据位序：LSB优先（低位在前）
 * - 结果处理：无需反转字节顺序
 *
 * @param data 待校验数据的指针
 * @param length 数据长度（字节数）
 * @return uint16_t 计算得到的CRC16值（低字节在前，符合Modbus规范）
 */
uint16_t cal_crc16_modbus(uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;  // 初始化CRC寄存器
    
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];       // 将数据字节与CRC低8位异或
        
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {          // 如果最低位为1
                crc = (crc >> 1) ^ 0xA001;  // 右移1位后与多项式异或
            } else {
                crc >>= 1;               // 仅右移1位
            }
        }
    }
    
    // 注意：Modbus协议要求CRC字节序为低字节在前
    // 如需交换字节顺序（高字节在前），可取消下一行注释
    // crc = (crc >> 8) | (crc << 8);
    
    return crc;
}

int cmd_buffer_check(uint8_t *cmd_buffer, size_t len, CHECK_TYPE_T check_type)
{
    uint16_t checksum_value = cmd_buffer[len - 2] | (cmd_buffer[len - 1] << 8);

    switch (check_type)
    {
        case CHECK_SUM:
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
            break;

        case CRC16_MODBUS:
            if (checksum_value != cal_crc16_modbus(cmd_buffer, len - 2))
            {
                ESP_LOGE(__FUNCTION__, "check crc16_modebus err! checksum_value 0x%x != 0x%x",
                        checksum_value,
                        cal_crc16_modbus(cmd_buffer, len - 2));
                return -1;
            }
            else {
                return 0;
            }

            break;

        default:
    }

    return -1;
}

#define CMD_HEADER_LEN 6

uint32_t send_ai_dream_body_raw_data(uart_port_t uart_port)
{
    uint8_t body_cmd[] = {0xA5, 0x5A, 0x02, 0x13, 0x00, 0x3D,
                          0X7B, 0X22, 0X70, 0X69, 0X63, 0X6B,
                          0X5F, 0X73, 0X77, 0X22, 0X3A, 0X31,
                          0X7D, 0XE8, 0X05};
    uart_write_bytes(uart_port, body_cmd, sizeof(body_cmd));

    return 0;
}

uint32_t get_pressure_sum_value(uint8_t *cmd_buffer, size_t len, CHECK_TYPE_T check_type)
{
    uint32_t sum_value = 0;

    switch (check_type)
    {
        case CHECK_SUM:
            sum_value = cmd_buffer[len - 2] | (cmd_buffer[len - 1] << 8);
            for(int i = 0; i < CMD_HEADER_LEN; ++i)
            {
                sum_value -= cmd_buffer[i];
            }
        break;

        case CRC16_MODBUS:
            sum_value = cal_check_sum(cmd_buffer + CMD_HEADER_LEN, len - CMD_HEADER_LEN - 2);
        break;
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
        uart_port_t uart_port,
        CHECK_TYPE_T check_type)
{
    uint8_t cmd_header_flag = 0x00;
    size_t rx_buffer_size = 0x00, real_rx_size = 0;
    int ret = 0;
    *cmd_size = 0x00;
    uint8_t receive_count = 0x00;

rework_port_read:
    if (receive_count >= 5)
    {
        rx_buffer_size = 0x00;
        goto out_parser;
    }
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
    if (((real_rx_size += CMD_HEADER_LEN) != (rx_buffer_size)) || (cmd_buffer_check(cmd_buffer, rx_buffer_size, check_type) != 0))
    {
        ESP_LOGE(__FUNCTION__, "CMD recive ERR real_rx_size %d != rx_buffer_size %d", real_rx_size, rx_buffer_size);
        receive_count ++;
        goto rework_port_read;
    }
    else
    {
        *cmd_size = rx_buffer_size;
    }

out_parser:
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
//#define PRESS_COM_BPS 115200

void pressure_thread(void *pvparams)
{
    uint8_t *usart1_rx_buffer = pvPortMalloc(UART_CACHE_BUFFER_SIZE);
    thread_pvparam_t *thread_pvparam = (thread_pvparam_t *)pvparams;
    queue_pressure_t queue_pressure = {0};
    int rt_value = 0x00;
    uint8_t port_count = 0;
    uint8_t count = 0;
    size_t rx_buffer_size = 0x00;
    EventBits_t pressure_raw_upload_event_bit = 0x00;
    uint64_t usart_parser_event_bit = 0x00;
    uint8_t send_cmd_enable = 0;
    CHECK_TYPE_T check_type = CHECK_SUM;

    thread_pvparam->queue_pressure = xQueueCreate(1, sizeof(queue_pressure_t));

    for (count = 0; count <= (USART_NUM_7 - USART_NUM_0); count ++)
    {
        queue_pressure.pressure_context[count].pressure_buffer = pvPortMalloc(UART_CACHE_BUFFER_SIZE);
        if (queue_pressure.pressure_context[count].pressure_buffer)
        {
            ESP_LOGI(__FUNCTION__, "malloc buffer %p count %d", queue_pressure.pressure_context[count].pressure_buffer, count);
            queue_pressure.pressure_context[count].pressure_data_size = UART_CACHE_BUFFER_SIZE;
        } else {
            queue_pressure.pressure_context[count].pressure_data_size = 0;
            ESP_LOGI(__FUNCTION__, "malloc buffer err! count %d", count);
        }
    }

    thread_pvparam->pressure_raw_upload_event = xEventGroupCreate();
    thread_pvparam->usart_parser_cmd_type_event = xEventGroupCreate();

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

    while(1)
    {

        usart_parser_event_bit = xEventGroupWaitBits(thread_pvparam->usart_parser_cmd_type_event,
                PARSER_USART_PROTOCOL_NORMAL | PARSER_USART_PROTOCOL_AI_DREAM,
                pdTRUE,
                pdFALSE,
                0);

        switch (usart_parser_event_bit)
        {
            case PARSER_USART_PROTOCOL_NORMAL:
                send_cmd_enable = 0;
                check_type = CHECK_SUM;
                break;

            case PARSER_USART_PROTOCOL_AI_DREAM:
                check_type = CRC16_MODBUS;
                send_cmd_enable = 1;
                break;

            default:
                break;
        }

        for (port_count = 0; port_count < PORT_USART_NUM; port_count ++)
        {
            pressure_raw_upload_event_bit = xEventGroupWaitBits(thread_pvparam->pressure_raw_upload_event,
                    0xff,
                    pdTRUE,
                    pdFALSE,
                    0);

            if (pressure_raw_upload_event_bit & 0xff)
            {
                xEventGroupSetBits(thread_pvparam->pressure_raw_upload_event, (pressure_raw_upload_event_bit & 0xff) << 8);
                ESP_LOGI(__FUNCTION__, "receive com upload 0x%lx", (uint32_t)pressure_raw_upload_event_bit - 1);
                xQueueOverwrite(thread_pvparam->queue_pressure, &queue_pressure);
            }


            rs2251_usart_ctrl_channel(rs2251_usart_range[port_count], true, true);
            vTaskDelay(50);
            uart_flush(SENSOR_INPUT_PORT);

            if (send_cmd_enable)
                send_ai_dream_body_raw_data(SENSOR_INPUT_PORT);

            rt_value = pressure_parser_form_uart(usart1_rx_buffer,
                    UART_CACHE_BUFFER_SIZE,
                    &rx_buffer_size,
                    SENSOR_INPUT_PORT,
                    check_type);
            if (rt_value > 0)
            {
                queue_pressure.port_connect_flag |= (0x01 << port_count);
                queue_pressure.pressure_context[port_count].pressure_sum_value =
                    get_pressure_sum_value(usart1_rx_buffer, rx_buffer_size, check_type);

                if (queue_pressure.pressure_context[port_count].pressure_buffer)
                {
                    memcpy(queue_pressure.pressure_context[port_count].pressure_buffer, usart1_rx_buffer, rx_buffer_size);
                    queue_pressure.pressure_context[port_count].pressure_data_size = rx_buffer_size;
                } else {
                    queue_pressure.pressure_context[port_count].pressure_data_size = 0x00;
                }
            }

            if (rt_value <= 0)
            {
                queue_pressure.port_connect_flag &= (~(0x01 << port_count));
                queue_pressure.pressure_context[port_count].pressure_sum_value = 0x00;
                queue_pressure.pressure_context[port_count].pressure_data_size = 0x00;
                memset(queue_pressure.pressure_context[port_count].pressure_buffer,
                       0x00,
                       queue_pressure.pressure_context[port_count].pressure_buffer_size);
#ifdef DEBUG_UART_DATA
                //ESP_LOGI(__FUNCTION__, "pop pressure %d value err! ret = %d\n", port_count, rt_value);
#endif
            }
        }

        queue_pressure.com_bsp = PRESS_COM_BPS;
        xQueueOverwrite(thread_pvparam->queue_pressure, &queue_pressure);

#ifdef DEBUG_UART_DATA
        for (port_count = 0; port_count < PORT_USART_NUM; port_count ++)
        {
            if ((queue_pressure.port_connect_flag & (0x01 << port_count)) == (0x01 << port_count))
            {
                ESP_LOGI(__FUNCTION__, "pressure_num %d value %ld",
                        port_count,
                        queue_pressure.pressure_context[port_count].pressure_sum_value
                        );
            }
        }
#endif
    }
}
