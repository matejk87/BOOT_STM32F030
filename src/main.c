#include <stddef.h>
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "printf.h"
#include "usart1.h"
#include "usart2.h"
#include "getline.h"
#include "timer.h"
#include "config.h"
#include "mybus.h"
#include "conf_IAP.h"
#include "bootloader.h"

extern unsigned char app_version_data[];

uint32_t timer=0;
uint8_t  timerFlag=0;
t_timer timer_led;

void init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
		RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 	// enable the clock to GPIOC			//(RM0091 lists this as IOPCEN, not GPIOCEN)
		RCC->AHBENR |= RCC_AHBENR_GPIOAEN; 	// enable the clock to GPIOC			//(RM0091 lists this as IOPCEN, not GPIOCEN)
		RCC->AHBENR |= RCC_AHBENR_GPIOBEN; 	// enable the clock to GPIOC			//(RM0091 lists this as IOPCEN, not GPIOCEN)
		GPIOB->MODER = (1 << 2);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		USART_InitTypeDef USART_InitStructure;

		  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
		  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
		  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
		  //Configure USART2 pins:  Rx and Tx ----------------------------
		  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3;
		  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		  GPIO_Init(GPIOA, &GPIO_InitStructure);
		  //Configure USART2 setting:         ----------------------------
		  USART_InitStructure.USART_BaudRate = 115200;
		  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		  USART_InitStructure.USART_StopBits = USART_StopBits_1;
		  USART_InitStructure.USART_Parity = USART_Parity_No;
		  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		  USART_Init(USART2, &USART_InitStructure);
		  USART_Cmd(USART2,ENABLE);

}


void ui_main(void)
{
/*	static unsigned long tmp32;
	static char rx_char;
	if(usart1_rxlen())
	{
		rx_char=usart1_getchar();
		if(rx_char=='r'||rx_char=='R')
		{
			printf("\n\r reset!");
			IWDG_Enable();
			while(1);
		}
		else if(rx_char=='x'||rx_char=='X')
		{
			printf("\n\r Xmodem bootloader ('esc' or 'e' to exit)");
			xmodem_bootloader=1;
			//print_usart(3); // todo print usart channel
			xm_start();
			//exit_BL();
		}
		else if(rx_char=='l'||rx_char=='L')
		{
			printf("\n\r lunch application!");
			exit_BL();
		}
		else if(rx_char=='i'||rx_char=='I')
		{
			printf("\n\rRS485 addr:0x");
			print_16(mod_addr);
			printf("\n\r BL  info: ");
			printf(VER_OPIS);
			printf("\n\r HW  info: ");
			printf(HARDWARE_INFO);
			printf("\n\r APP info: ");
		    if(search_app_version( PROGRAM_START_ADDRESS,STM32F_FLASH_ADDR+STM32F_FLASH_SIZE, app_version_data,0))
            {
            	app_version_data[99]=0;//varovalka
            	printf(app_version_data);
            }
            else putstr("no application!");

		}
		else if(rx_char=='e'||rx_char=='E')
		{
			printf("\n\r Exit bootloader");
			exit_BL();
		}
		else if(rx_char=='d'||rx_char=='D')
		{
			printf("\n\r debug");
			tmp32 = *((unsigned long*) MYISP_FB_SWSEL_ADDR);
			printf("\n\r fuse_word:0x");
			print_32(tmp32);

			tmp32 = *((unsigned long*) MYISP_FB_SWSEL_ADDR+4);
			printf("\n\r fuse_word2:0x");
			print_32(tmp32);
		}
		else if(rx_char=='m'||rx_char=='M')print_main_menu();
		else if(rx_char==13){}
		else
		{
			printf("");
			usart1_putchar(rx_char);
			printf(" is unknown command!");
		}
		printf("\n\r=>");
	}*/
}

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
	unsigned long data_offset=0;
	unsigned long data_addr=0;
  int ii=0;
  /* TODO - Add your application code here */
  SysTick_Config(4800);  /* 0.1 ms = 100us if clock frequency 12 MHz */
  SystemCoreClockUpdate();
  ii = SystemCoreClock;    /* This is a way to read the System core clock */
  ii = 0;
  init();
  timer_init(1000);
  timeout(&timer_led,0);
  usart2_init();
  mybus_init();
  printf("\n\n\n");
  printf(VER_OPIS);
  while (1)
  {
	  if(timeout(&timer_led,50))
	  {
	  	GPIOB->ODR ^= GPIO_Pin_1;
	  }
	  getline();
	  mybus_main();
  }
  return 0;
}


int cmd_reset(int argc,char ** argv)
{
	resetcpu();
	return 1;
}
