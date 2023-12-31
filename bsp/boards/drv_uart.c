/**
 * @file drv_uart.c
 * @author brimonzzy (zzybrimon@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "drv_uart.h"


static uint8_t usart6_rx_buff[USART6_RX_BUFFER_SIZE];
static uint8_t usart6_tx_buff[USART6_TX_BUFFER_SIZE];
static uint8_t usart6_tx_fifo_buff[USART6_TX_FIFO_SIZE];

static uint8_t usart1_rx_buff[USART1_RX_BUFFER_SIZE];
static uint8_t usart1_tx_buff[USART1_TX_BUFFER_SIZE];
static uint8_t usart1_tx_fifo_buff[USART1_TX_FIFO_SIZE];

usart_manage_obj_t usart1_manage_obj = {0};
usart_manage_obj_t usart6_manage_obj = {0};

static void usart_rec_to_buff(usart_manage_obj_t *m_obj, interrput_type int_type);
static void usart_transmit_hook(usart_manage_obj_t *m_obj);


void debug_raw_printf(char *fmt, ...)
{
    va_list arg;
    uint8_t buff[USART1_PRINTF_BUFF_SIZE];
    uint8_t printf_len;

    va_start(arg, fmt);
    printf_len = vsnprintf((char *)buff, USART1_PRINTF_BUFF_SIZE, fmt, arg);
    va_end(arg);

    if (printf_len > USART1_PRINTF_BUFF_SIZE) {
        printf_len = USART1_PRINTF_BUFF_SIZE;
    }

    HAL_UART_Transmit(&huart1, (uint8_t *)buff, printf_len, 0xFFFF);
}

/* USART1 IDLE callback */
void usart1_idle_callback(void)
{
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
        usart_rec_to_buff(&usart1_manage_obj, INTERRUPT_TYPE_UART);
    }
}

void usart6_idle_callback(void)
{
    if (__HAL_UART_GET_FLAG(&huart6, UART_FLAG_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(&huart6);
        usart_rec_to_buff(&usart6_manage_obj, INTERRUPT_TYPE_UART);
    }
}

/* initial the usart manage */

void usart1_manage_init(void)
{
    usart1_manage_obj.rx_buffer = usart1_rx_buff;
    usart1_manage_obj.rx_buffer_size = USART1_RX_BUFFER_SIZE;
    usart1_manage_obj.dma_h = &hdma_usart1_rx;
    usart1_manage_obj.uart_h = &huart1;
    usart1_manage_obj.tx_fifo_buffer = usart1_tx_fifo_buff;
    usart1_manage_obj.tx_fifo_size = USART1_TX_FIFO_SIZE;
    usart1_manage_obj.tx_buffer_size = USART1_TX_BUFFER_SIZE;
    usart1_manage_obj.tx_buffer = usart1_tx_buff;
    usart1_manage_obj.is_sending = 0;

    fifo_s_init(&(usart1_manage_obj.tx_fifo), usart1_tx_fifo_buff, USART1_TX_FIFO_SIZE);

    HAL_UART_Receive_DMA(&huart1, usart1_rx_buff, USART1_RX_BUFFER_SIZE);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
}

void usart6_manage_init(void)
{
    usart6_manage_obj.rx_buffer = usart6_rx_buff;
    usart6_manage_obj.rx_buffer_size = USART6_RX_BUFFER_SIZE;
    usart6_manage_obj.dma_h = &hdma_usart6_rx;
    usart6_manage_obj.uart_h = &huart6;
    usart6_manage_obj.tx_fifo_buffer = usart6_tx_fifo_buff;
    usart6_manage_obj.tx_fifo_size = USART6_TX_FIFO_SIZE;
    usart6_manage_obj.tx_buffer_size = USART6_TX_BUFFER_SIZE;
    usart6_manage_obj.tx_buffer = usart6_tx_buff;
    usart6_manage_obj.is_sending = 0;

    fifo_s_init(&(usart6_manage_obj.tx_fifo), usart6_tx_fifo_buff, USART6_TX_FIFO_SIZE);

    HAL_UART_Receive_DMA(&huart6, usart6_rx_buff, USART6_RX_BUFFER_SIZE);
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
}

int usart1_printf(char *fmt, ...)
{
    va_list arg;
    uint8_t buff[USART1_PRINTF_BUFF_SIZE];
    uint8_t printf_len;

    va_start(arg, fmt);
    printf_len = vsnprintf((char *)buff, USART1_PRINTF_BUFF_SIZE, fmt, arg);
    va_end(arg);

    if (printf_len > USART1_PRINTF_BUFF_SIZE) {
        printf_len = USART1_PRINTF_BUFF_SIZE;
    }

    usart_transmit(&usart1_manage_obj, buff, printf_len);

    return 0;
}

void usart1_transmit(uint8_t *buff, uint16_t len)
{
    usart_transmit(&usart1_manage_obj, buff, len);
}

void usart6_transmit(uint8_t *buff, uint16_t len)
{
    usart_transmit(&usart6_manage_obj, buff, len);
}

void usart1_rx_callback_register(usart_call_back fun)
{
    usart_rx_callback_register(&usart1_manage_obj, fun);
    return;
}

void usart6_rx_callback_register(usart_call_back fun)
{
    usart_rx_callback_register(&usart6_manage_obj, fun);
    return;
}

/********************** Usual API **********************************************/
/**
  * @brief  register uart callback function.
  * @param
  * @retval void
  */
void usart_rx_callback_register(usart_manage_obj_t *m_obj, usart_call_back fun)
{
    m_obj->call_back_f = fun;
    return;
}

/**
  * @brief  rx dma half complete interupt
  * @param
  * @retval void
  */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        usart_rec_to_buff(&usart1_manage_obj, INTERRUPT_TYPE_DMA_HALF);
    }
    else if (huart == &huart6)
    {
        usart_rec_to_buff(&usart6_manage_obj, INTERRUPT_TYPE_DMA_HALF);
    }

    return;
}

/**
  * @brief  rx dma complete interupt
  * @param
  * @retval void
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        usart_rec_to_buff(&usart1_manage_obj, INTERRUPT_TYPE_DMA_ALL);
    }
    else if (huart == &huart6)
    {
        usart_rec_to_buff(&usart6_manage_obj, INTERRUPT_TYPE_DMA_ALL);
    }

    return;
}

/**
  * @brief  tx complete interupt
  * @param
  * @retval void
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        usart_transmit_hook(&usart1_manage_obj);
    }
    else if (huart == &huart6)
    {
        usart_transmit_hook(&usart6_manage_obj);
    }

    return;
}

/**
  * @brief  uart fifo transmit
  * @param
  * @retval error code
  */
int32_t usart_transmit(usart_manage_obj_t *m_obj, uint8_t *buf, uint16_t len)
{
    uint16_t to_send_len;
    uint16_t to_tx_fifo_len;

    if (m_obj->is_sending == 0)
    {
        if (len < m_obj->tx_buffer_size)
        {
            to_send_len = len;
            to_tx_fifo_len = 0;
        }
        else if (len < m_obj->tx_buffer_size + m_obj->tx_fifo_size)
        {
            to_send_len = m_obj->tx_buffer_size;
            to_tx_fifo_len = len - m_obj->tx_buffer_size;
        }
        else
        {
            to_send_len = m_obj->tx_buffer_size;
            to_tx_fifo_len = m_obj->tx_fifo_size;
        }
    }
    else
    {
        if (len < m_obj->tx_fifo_size)
        {
            to_send_len = 0;
            to_tx_fifo_len = len;
        }
        else
        {
            to_send_len = 0;
            to_tx_fifo_len = m_obj->tx_fifo_size;
        }
    }

    if (to_send_len > 0)
    {
        memcpy(m_obj->tx_buffer, buf, to_send_len);
        m_obj->is_sending = 1;
        HAL_UART_Transmit_DMA(m_obj->uart_h, m_obj->tx_buffer, to_send_len);
    }
    if (to_tx_fifo_len > 0)
    {
        uint8_t len;
        len = fifo_s_puts(&(m_obj->tx_fifo), (char *)(buf) + to_send_len, to_tx_fifo_len);

        if (len != to_tx_fifo_len)
        {
            return -1;
        }
    }

    return 0;
}

static void usart_transmit_hook(usart_manage_obj_t *m_obj)
{
    uint16_t fifo_data_num = 0;
    uint16_t send_num = 0;

    fifo_data_num = m_obj->tx_fifo.used_num;

    if (fifo_data_num != 0)
    {
        if (fifo_data_num < m_obj->tx_buffer_size)
        {
            send_num = fifo_data_num;
        }
        else
        {
            send_num = m_obj->tx_buffer_size;
        }
        fifo_s_gets(&(m_obj->tx_fifo), (char *)(m_obj->tx_buffer), send_num);
        m_obj->is_sending = 1;
        HAL_UART_Transmit_DMA(m_obj->uart_h, m_obj->tx_buffer, send_num);
    }
    else
    {
        m_obj->is_sending = 0;
    }
    return;
}

/**
  * @brief  rx to fifo
  * @param
  * @retval void
  */
static void usart_rec_to_buff(usart_manage_obj_t *m_obj, interrput_type int_type)
{
    uint16_t read_end_ptr = 0;
    uint16_t read_length = 0;
    uint16_t read_success_length = 0;
    uint16_t read_start_ptr = m_obj->read_start_index;
    uint8_t *pdata = m_obj->rx_buffer;

    UNUSED(read_success_length);

    uint16_t buff_left = m_obj->dma_h->Instance->NDTR;

    if (int_type == INTERRUPT_TYPE_UART)
    {
        read_end_ptr = m_obj->rx_buffer_size - buff_left;
    }

    if (int_type == INTERRUPT_TYPE_DMA_HALF)
    {
        read_end_ptr = m_obj->rx_buffer_size / 2;
    }

    if (int_type == INTERRUPT_TYPE_DMA_ALL)
    {
        read_end_ptr = m_obj->rx_buffer_size;
    }

    read_length = read_end_ptr - m_obj->read_start_index;

    if (m_obj->call_back_f != NULL)
    {
        uint8_t *read_ptr = pdata + read_start_ptr;
        read_success_length = m_obj->call_back_f(read_ptr, read_length);
    }

    m_obj->read_start_index = (m_obj->read_start_index + read_length) % (m_obj->rx_buffer_size);

    return;
}
