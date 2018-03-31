#include "usart.h"
#include "stm32f0xx_conf.h"
#include "printf.h"

#define USART1_RX_BUFF_SIZE 255
#define USART1_TX_BUFF_SIZE 1000

char usart1_rx_fifo[USART1_RX_BUFF_SIZE];
char usart1_tx_fifo[USART1_TX_BUFF_SIZE];

t_usart_fifo usart1_fifo=
{
		.Rx_buf=usart1_rx_fifo,  // sprejemni buffer
		.Tx_buf=usart1_tx_fifo  // oddajni buffer

};

const t_usart_hwdef usart1_hw=
{
//.COM_USART=USART1,
.COM_USART_CLK=RCC_APB2Periph_USART1,
.COM_TX_PORT = GPIOA,
.COM_TX_PIN =       GPIO_Pin_10,
.COM_TX_PORT_CLK =  RCC_AHBPeriph_GPIOA,
.COM_TX_PIN_SOURCE =GPIO_PinSource10,
.COM_TX_AF =         GPIO_AF_1,
.COM_RX_PORT = GPIOA,
.COM_RX_PIN =       GPIO_Pin_9,
.COM_RX_PORT_CLK =  RCC_AHBPeriph_GPIOA,
.COM_RX_PIN_SOURCE  = GPIO_PinSource9,
.COM_RX_AF=         GPIO_AF_1,
.COM_RTS_PORT = GPIOA,
.COM_RTS_PIN =       GPIO_Pin_12,
.COM_RTS_PORT_CLK =  RCC_AHBPeriph_GPIOA,
.COM_RTS_PIN_SOURCE  = GPIO_PinSource12,
.COM_RTS_AF=         GPIO_AF_1
};

const t_usart usart1 =
{
	.hwdef=&usart1_hw,
	.fifo=&usart1_fifo,
	.RXBUFFLEN=USART1_RX_BUFF_SIZE,
	.TXBUFFLEN=USART1_TX_BUFF_SIZE,
    .usartx=USART1

};

void USART1_IRQHandler(void)
{
	USART_IRQHandler(&usart1);
}


static void NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; todo on m3 and m4 devices
 // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
};

void usart1_init(t_usart * usart)
{
	/* USARTx configured as follow:
	        - BaudRate = 115200 baud
	        - Word Length = 8 Bits
	        - One Stop Bit
	        - No parity
	        - Hardware flow control disabled (RTS and CTS signals)
	        - Receive and transmit enabled
	  */
	  const USART_InitTypeDef USART_InitStructure={
	  .USART_BaudRate = 115200,
	  .USART_WordLength = USART_WordLength_8b,
	  .USART_StopBits = USART_StopBits_1,
	  .USART_Parity = USART_Parity_No,
	  .USART_HardwareFlowControl = USART_HardwareFlowControl_RTS,
	  .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
	  };
	  NVIC_Config();
	  USART_init(&usart1, (USART_InitTypeDef *) &USART_InitStructure);
	 // printf("\nTest Run");

}

void usart1_deinit(void)
{

	/*  const USART_InitTypeDef USART_InitStructure={
	  .USART_BaudRate = 115200,
	  .USART_WordLength = USART_WordLength_8b,
	  .USART_StopBits = USART_StopBits_1,
	  .USART_Parity = USART_Parity_No,
	  .USART_HardwareFlowControl = USART_HardwareFlowControl_RTS,
	  .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
	  };*/
	 //NVIC_Config();
	  NVIC_InitTypeDef NVIC_InitStructure;
	  /* Enable the USART1 Interrupt */
	  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; todo on m3 and m4 devices
	 // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;//ENABLE;
	  NVIC_Init(&NVIC_InitStructure);

	//  USART_init(&usart1, (USART_InitTypeDef *) &USART_InitStructure);
	 // printf("\nTest Run");

}


void usart1_putchar(char znak)
{
	//todo preskoèi èe je buffer poven (samo pri usart1!)
	 USART_putchar(&usart1,znak);
}
char usart1_getchar()
{
	return USART_getchar(&usart1);
}
int usart1_rxlen()
{

   return USART_rbufflen(&usart1);
}
int usart1_txlen()
{

   return USART_tbufflen(&usart1);
}


