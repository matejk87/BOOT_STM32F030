
//	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#include "usart.h"
#include "stm32f0xx_conf.h"
#include "printf.h"

#define USART2_RX_BUFF_SIZE 255
#define USART2_TX_BUFF_SIZE 255

char usart2_rx_fifo[USART2_RX_BUFF_SIZE];
char usart2_tx_fifo[USART2_TX_BUFF_SIZE];

t_usart_fifo usart2_fifo=
{
		.Rx_buf=usart2_rx_fifo,  // sprejemni buffer
		.Tx_buf=usart2_tx_fifo  // oddajni buffer
};

const t_usart_hwdef usart2_hw=
{
.COM_USART_CLK=RCC_APB1Periph_USART2,
.COM_TX_PORT = GPIOA,
.COM_TX_PIN =       GPIO_Pin_2,
.COM_TX_PORT_CLK =  RCC_AHBPeriph_GPIOA,
.COM_TX_PIN_SOURCE =GPIO_PinSource2,
.COM_TX_AF =         GPIO_AF_1,
.COM_RX_PORT = GPIOA,
.COM_RX_PIN =       GPIO_Pin_3,
.COM_RX_PORT_CLK =  RCC_AHBPeriph_GPIOA,
.COM_RX_PIN_SOURCE  = GPIO_PinSource3,
.COM_RX_AF=         GPIO_AF_1
/*.COM_RTS_PORT = GPIOB,
.COM_RTS_PIN =       GPIO_Pin_14,
.COM_RTS_PORT_CLK =  RCC_AHBPeriph_GPIOB,
.COM_RTS_PIN_SOURCE  = GPIO_PinSource14,
.COM_RTS_AF=         GPIO_AF_7*/
};

const t_usart usart2 =
{
	.hwdef=&usart2_hw,
	.fifo=&usart2_fifo,
	.RXBUFFLEN=USART2_RX_BUFF_SIZE,
	.TXBUFFLEN=USART2_TX_BUFF_SIZE,
    .usartx=USART2
};

void USART2_IRQHandler(void)
{
	/*printf("\n rx3 char");
		STM_EVAL_LEDOn(LED4);*/
	USART_IRQHandler(&usart2);
}

static void NVIC_Config_usart2(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USART3 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
 /* NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1 ; //todo on m3 and m4 devices
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;*/
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
};

void usart2_init(t_usart * usart)
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
	  .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
	  .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
	  };
	  NVIC_Config_usart2();
	  USART_init(&usart2, (USART_InitTypeDef *) &USART_InitStructure);
	  printf("\nusart2 init");
}

void usart2_putchar(char znak)
{
	 USART_putchar(&usart2,znak);
}
char usart2_getchar()
{
	return USART_getchar(&usart2);
}
int usart2_rxlen()
{
   return USART_rbufflen(&usart2);
}




