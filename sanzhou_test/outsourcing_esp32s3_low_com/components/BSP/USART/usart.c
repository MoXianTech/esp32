#include "usart.h"




/**
 * @brief       初始化串口
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @note        注意: 必须设置正确的时钟源, 否则串口波特率就会设置异常.
 * @retval      无
 */
void usart_init(uint32_t baudrate,
                uart_port_t uart_port,
                gpio_num_t gpio_rx,
                gpio_num_t gpio_tx,
                QueueHandle_t *uart_queue)
{
    uart_config_t uart_config;                          /* 串口配置句柄 */

    uart_config.baud_rate = baudrate;                   /* 波特率 */
    uart_config.data_bits = UART_DATA_8_BITS;           /* 字长为8位数据格式 */
    uart_config.parity = UART_PARITY_DISABLE;           /* 无奇偶校验位 */
    uart_config.stop_bits = UART_STOP_BITS_1;           /* 一个停止位 */
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;   /* 无硬件控制流 */
    uart_config.source_clk = UART_SCLK_APB;             /* 配置时钟源 */
    uart_config.rx_flow_ctrl_thresh = 122;              /* 硬件控制流阈值 */
    uart_param_config(uart_port, &uart_config);          /* 配置uart端口 */


    /* 配置uart引脚 */
    uart_set_pin(uart_port, gpio_tx, gpio_rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    /* 安装串口驱动 */
    uart_driver_install(uart_port, RX_BUF_SIZE * 4, TX_BUF_SIZE * 2, 20, uart_queue, 0);
}


void usart_deinit(uart_port_t uart_port)
{
    /* 删除串口驱动 */
    uart_driver_delete(uart_port);
}
