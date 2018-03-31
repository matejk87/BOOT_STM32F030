
#ifndef USART_H_
#define USART_H_
#include "stm32f0xx.h"


typedef struct {
	volatile unsigned int Rx_in;  	// kazalec na sprejemne podatke vhod v buffer
	volatile unsigned int Rx_out;	// kazalec na sprejemne podatke izhod iz buffer
	volatile unsigned int Tx_in; 	// kazalec na oodajne podatke vhod v buffer
	volatile unsigned int Tx_out;	// kazalec na oodajne podatke izhod iz buffer
	volatile char * Rx_buf;  // sprejemni buffer
	volatile char * Tx_buf;  // oddajni buffer
} t_usart_fifo;

t_usart_fifo usart1_fifo;


typedef struct
{
	const uint32_t COM_USART_CLK;
	GPIO_TypeDef* COM_TX_PORT;
	const uint16_t COM_TX_PIN;
	const uint32_t COM_TX_PORT_CLK;
	const uint16_t COM_TX_PIN_SOURCE;
	const uint8_t COM_TX_AF;
	GPIO_TypeDef* COM_RX_PORT;
	const uint16_t COM_RX_PIN;
	const uint32_t COM_RX_PORT_CLK;
	const uint16_t COM_RX_PIN_SOURCE;
	const uint8_t COM_RX_AF;
	GPIO_TypeDef* COM_RTS_PORT;
	const uint16_t COM_RTS_PIN;
	const uint32_t COM_RTS_PORT_CLK;
	const uint16_t COM_RTS_PIN_SOURCE;
	const uint8_t COM_RTS_AF;
}t_usart_hwdef;


typedef struct
{
	const t_usart_hwdef * hwdef;
	t_usart_fifo * fifo;
    USART_TypeDef * usartx;
	int RXBUFFLEN;
	int TXBUFFLEN;
} t_usart;


void USART_init(const t_usart * usart, USART_InitTypeDef* USART_InitStruct);
int USART_rbufflen(const t_usart * usart);
int USART_tbufflen(const t_usart * usart);
int USART_putchar(const t_usart * usart,char znak);
int USART_getchar(const t_usart * usart);
void USART_IRQHandler(const t_usart * usart);


#endif /* UART_H_ */
