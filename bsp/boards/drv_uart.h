#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#include "dma.h"
#include "usart.h"
#include "drv_uart.h"
#include "fifo.h"
#include "sys.h"


#define USART1_RX_BUFFER_SIZE (512)
#define USART1_TX_BUFFER_SIZE (512)
#define USART1_TX_FIFO_SIZE (2048)

#define USART6_RX_BUFFER_SIZE (512)
#define USART6_TX_BUFFER_SIZE (512)
#define USART6_TX_FIFO_SIZE (1024)

#define USART1_PRINTF_BUFF_SIZE (128)

extern DMA_HandleTypeDef hdma_usart6_rx;
extern DMA_HandleTypeDef hdma_usart6_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;


typedef uint32_t (*usart_call_back)(uint8_t *buf, uint16_t len);

typedef struct {
    UART_HandleTypeDef *uart_h;
    DMA_HandleTypeDef *dma_h;
	/* RX */
    uint16_t rx_buffer_size;
    uint8_t *rx_buffer;
    uint16_t read_start_index;
    usart_call_back call_back_f;
	
	/* TX */
    uint8_t *tx_buffer;
    uint16_t tx_buffer_size;
    fifo_s_t tx_fifo;
    uint8_t *tx_fifo_buffer;
    uint16_t tx_fifo_size;
    uint8_t is_sending;
} usart_manage_obj_t;

typedef enum {
    INTERRUPT_TYPE_UART = 0,
    INTERRUPT_TYPE_DMA_HALF = 1,
    INTERRUPT_TYPE_DMA_ALL = 2
} interrput_type;


void usart_rx_callback_register(usart_manage_obj_t *m_obj, usart_call_back fun);
int32_t usart_transmit(usart_manage_obj_t *m_obj, uint8_t *buf, uint16_t len);
void usart1_manage_init(void);
void usart6_manage_init(void);
void usart1_transmit(uint8_t *buff, uint16_t len);
void usart6_transmit(uint8_t *buff, uint16_t len);
void usart1_idle_callback(void);
void usart6_idle_callback(void);
uint32_t uart6_rx_data_handle(uint8_t *buff, uint32_t len);
void usart1_rx_callback_register(usart_call_back fun);
void usart6_rx_callback_register(usart_call_back fun);
void debug_raw_printf(char *fmt, ...);

#endif // __DRV_UART_H__
