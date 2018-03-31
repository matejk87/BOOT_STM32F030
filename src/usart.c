
#include "stm32f0xx_conf.h"
#include "usart.h"
#include "usart1.h"

void USART_init(const t_usart * usart, USART_InitTypeDef* USART_InitStruct)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	usart->fifo->Rx_in=0;
	usart->fifo->Rx_out=0;
	usart->fifo->Tx_in=0;
	usart->fifo->Tx_out=0;

	/* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(usart->hwdef->COM_TX_PORT_CLK | usart->hwdef->COM_RX_PORT_CLK| usart->hwdef->COM_RTS_PORT_CLK, ENABLE);

	  /* Enable USART clock */
     //todo deluje samo za USART1 in USART3
	 if(usart->usartx==USART1) RCC_APB2PeriphClockCmd(usart->hwdef->COM_USART_CLK, ENABLE);
	 else RCC_APB1PeriphClockCmd(usart->hwdef->COM_USART_CLK, ENABLE);

	  /* Connect PXx to USARTx_Tx */
	 GPIO_PinAFConfig(usart->hwdef->COM_TX_PORT, usart->hwdef->COM_TX_PIN_SOURCE, usart->hwdef->COM_TX_AF);

	  /* Connect PXx to USARTx_Rx */
	 GPIO_PinAFConfig(usart->hwdef->COM_RX_PORT, usart->hwdef->COM_RX_PIN_SOURCE, usart->hwdef->COM_RX_AF);

	 //todo inicializacija RTS pina
	 // usart->hwdef->
	 if(usart->usartx==USART1)
	 {
		 GPIO_PinAFConfig(usart->hwdef->COM_RTS_PORT, usart->hwdef->COM_RTS_PIN_SOURCE, usart->hwdef->COM_RTS_AF);
		 USART_DECmd( usart->usartx, ENABLE);

	 // usart->hwdef->
	 }
	  /* Configure USART Tx as alternate function push-pull */
	 GPIO_InitStructure.GPIO_Pin =   usart->hwdef->COM_TX_PIN;
	 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
	 GPIO_Init(usart->hwdef->COM_TX_PORT, &GPIO_InitStructure);

	  /* Configure USART RTS*/
	 GPIO_InitStructure.GPIO_Pin =   usart->hwdef->COM_RTS_PIN;
	 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	 GPIO_Init(usart->hwdef->COM_RTS_PORT, &GPIO_InitStructure);

	  /* Configure USART Rx as alternate function push-pull */
	 GPIO_InitStructure.GPIO_Pin = usart->hwdef->COM_RX_PIN;
	 GPIO_Init(usart->hwdef->COM_RX_PORT, &GPIO_InitStructure);

	  /* USART configuration */
	 USART_Init(usart->usartx, USART_InitStruct);
     /* Enable USART */
	 USART_ITConfig(usart->usartx, USART_IT_RXNE, ENABLE);
	 USART_ITConfig(usart->usartx, USART_IT_TXE, ENABLE);

	 USART_Cmd(usart->usartx, ENABLE);
	/* putstr("\n\r CR1: 0x");
	 print_32(*((unsigned long *)0x40013800));
	 putstr("\n\r CR3: 0x");
	 print_32(*((unsigned long *)0x40013800));
	 putstr("\n\r CR1: 0x");
	 print_32(*((unsigned long *)0x40004800));
	 putstr("\n\r CR3: 0x");
	 print_32(*((unsigned long *)0x40004800));
*/
	/* putstr("\n\r CR1: 0x");
	 print_32(*((unsigned long *)0x40013800));
	 putstr("\n\r CR1: 0x");
	 print_32(*((unsigned long *)0x40013800));*/




}


void USART_IRQHandler(const t_usart * usart)
 {
   if(USART_GetITStatus(usart->usartx, USART_IT_RXNE) != RESET)
   {
	 char rx_znak;
	 rx_znak= (USART_ReceiveData(usart->usartx) & 0xFF);
	 if(USART_rbufflen(usart)<(usart->RXBUFFLEN-2)){
		usart->fifo->Rx_buf[usart->fifo->Rx_in % usart->RXBUFFLEN] =rx_znak;
		usart->fifo->Rx_in++;
	 }
   }
   if(USART_GetITStatus(usart->usartx, USART_IT_TXE) != RESET)
   {
	 /* Write one byte to the transmit data register */
	 if (usart->fifo->Tx_in != usart->fifo->Tx_out) {
	   USART_SendData(usart->usartx, usart->fifo->Tx_buf[(usart->fifo->Tx_out++)%usart->TXBUFFLEN]);
	 }
	 else
	 {
	   USART_ITConfig(usart->usartx, USART_IT_TXE, DISABLE);
	 }
   }
 }


int USART_rbufflen(const t_usart * usart)
{
	return usart->fifo->Rx_in-usart->fifo->Rx_out;
}

int USART_tbufflen(const t_usart * usart)
{
	return usart->fifo->Tx_in-usart->fifo->Tx_out;
}



int USART_putchar(const t_usart * usart,char znak)
{
	while(USART_tbufflen(usart)>=usart->TXBUFFLEN-2){};
	if(USART_tbufflen(usart)<usart->TXBUFFLEN-2)
	{
		usart->fifo->Tx_buf[usart->fifo->Tx_in % usart->TXBUFFLEN]=znak;
		usart->fifo->Tx_in++;
		USART_ITConfig(usart->usartx, USART_IT_TXE, ENABLE);
		return 1;
	}
	return 0;
}

int USART_getchar(const t_usart * usart)
{
	if (usart->fifo->Rx_in != usart->fifo->Rx_out) {
      return usart->fifo->Rx_buf[((usart->fifo->Rx_out++)%usart->RXBUFFLEN)];
	}
	return -1;
}



