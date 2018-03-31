/*
 * BL.c
 *
 *  Created on: 11. apr. 2013
 *      Author: Matej
 */

#include "BL.h"
#include "usart1.h"
#include "printf.h"
#include "conf_IAP.h"
#include "flash_if.h"
#include "crc16.h"
#include "config.h"
#include "timer.h"
#include "bootloader.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

char version_str[]="<BL_version_data>" VER_OPIS "</BL_version_data>";
char HW_version_str[]="<HW_version_data>" HARDWARE_INFO "</HW_version_data>";

unsigned short crc_calc;
unsigned long current_flash_addr;// naslov od zaèetka aplikacije naprej
unsigned char flash_buff[FLASH_PAGE_SIZE];
int flash_buff_cnt;
int page_cntr;

int flash_write(unsigned char *data,int datalen,unsigned long addr) //programsko kodo moramo zaokrožiti na page size(dopolnimo(padd) s poljubno cifro)
{
  int i,tmp;
  unsigned long flash_addr_container=0;
  if(addr == 0) //za primer da je naprava v bootloaderju ostala od prej
  {
    crc_calc=0;
    current_flash_addr=0;
    flash_buff_cnt=0;
    page_cntr=0;
  }
  if((current_flash_addr + flash_buff_cnt) == addr)
  {
    crc16_n(&crc_calc,(char *)data,datalen);

    for(i=0;i<datalen;i++)
    {
      flash_buff[flash_buff_cnt++] = data[i];
   /*   putstr(" ");
      print_08(data[i]);*/
    }
    if(flash_buff_cnt == FLASH_PAGE_SIZE )
    {
     // printf("\n erase_page:%d",(PROGRAM_START_OFFSET/FLASH_PAGE_SIZE)+page_cntr);
    //	putstr("\n\r erase page");
    	if((PROGRAM_START_ADDRESS+page_cntr*FLASH_PAGE_SIZE)>=CFG_PAGE_ADDR)
    	{
    		printf("\n\r flash_write address error!!!");
    		return 0;
    	}
    	printf("\n\r flash sector erase 0x%x",PROGRAM_START_ADDRESS+(page_cntr*FLASH_PAGE_SIZE));

    	/*
    	if(FLASH_If_Erase(PROGRAM_START_ADDRESS+(page_cntr*FLASH_PAGE_SIZE))==0)putstr("\n\r:OK");
    	else putstr("\n\r:ERROR");*/
    	//FLASH_Unlock();

    /*	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); //todo
    	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
    	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);*/
    	//delay_ms(100);
    	FLASH_If_Init();

    	tmp = FLASH_ErasePage(PROGRAM_START_ADDRESS+(page_cntr*FLASH_PAGE_SIZE));
    	/*USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//todo
    	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

    	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);*/
    	if(tmp==FLASH_COMPLETE)printf(":OK");
    	else
    	{
    		printf(":ERROR:0x%x",tmp);

    	}
    	// flashc_erase_page((PROGRAM_START_OFFSET/FLASH_PAGE_SIZE)+page_cntr , 0);
    //  printf("  f_write:%x",0x80000000+PROGRAM_START_OFFSET+page_cntr*FLASH_PAGE_SIZE);
    	flash_addr_container = PROGRAM_START_ADDRESS+page_cntr*FLASH_PAGE_SIZE;
    	printf(" fwrite:0x%x",flash_addr_container);

      FLASH_If_Write(&flash_addr_container,(unsigned long*)flash_buff,FLASH_PAGE_SIZE);
   //   memcpy_flash((void *)0x80000000+PROGRAM_START_OFFSET+page_cntr*FLASH_PAGE_SIZE,(void *)flash_buff ,FLASH_PAGE_SIZE,1);
      page_cntr++;
      flash_buff_cnt=0;
      current_flash_addr+=FLASH_PAGE_SIZE;
    }
    return 1;
  }
  else return 0;
}

int flash_write_last(void)
{
	int tmp = 0;
	unsigned long flash_addr_container;
	flash_addr_container = PROGRAM_START_ADDRESS+page_cntr*FLASH_PAGE_SIZE;
	printf("\n\r flash sector erase 0x%x",PROGRAM_START_ADDRESS+(page_cntr*FLASH_PAGE_SIZE));
	FLASH_If_Init();
   	tmp = FLASH_ErasePage(PROGRAM_START_ADDRESS+(page_cntr*FLASH_PAGE_SIZE));
   	if(tmp==FLASH_COMPLETE)printf(":OK");
   	else
   	{
   		printf(":ERROR:0x%x",tmp);
   	}
	printf("\n\r fwrite:0x%x",flash_addr_container);
	FLASH_If_Write(&flash_addr_container,(unsigned long*)flash_buff,FLASH_PAGE_SIZE);
	page_cntr++;
	return 1;
}



int flash_erase_to_end(void)
{
  printf("\n\rflash_erase_to_end"); //todo
  unsigned long data_addr;
 // printf("\npage_cntr:%d",page_cntr);
  if(page_cntr==0)return 1;
  unsigned char buff[100];
  if(search_app_version(PROGRAM_START_ADDRESS+((page_cntr)*FLASH_PAGE_SIZE),CFG_PAGE_ADDR, buff,&data_addr))
  {
	//  if(1)FLASH_Unlock();
	  printf("\n\r old version data addr:0x%x",data_addr);
	  printf("\n\r erasing page 0x%x",data_addr&0xfffff800);
	 // FLASH_Unlock();
	  FLASH_If_Init();
	  if(FLASH_ErasePage(data_addr&0xfffff800)==FLASH_COMPLETE)printf(" :OK");
	  else printf(" :ERROR");
	  FLASH_Lock();
	if(search_app_version( PROGRAM_START_ADDRESS +(page_cntr*FLASH_PAGE_SIZE),PROGRAM_END_ADDRESS,buff,&data_addr))
    {
      printf("\n\rold version data not deleted!");
    }
    else printf("\n\rold version data deleted!");
  }
  return 1;
}

void exit_BL(void)
{
	unsigned short rs485_addr=0;

	// printf("todo: enter bootloader!");
	unsigned long my_fuse[2];
	my_fuse[0]=0xffffffff;
	my_fuse[1]=get_RS485addr_from_flash();
	unsigned long addr=CFG_PAGE_ADDR;
	printf("\n going to application! RS485 addr:0x%x",rs485_addr);
	// printf("\n\r deleting BL fuse bit:");
	 delay(50);
	  /* Disable all interrupts */
	  RCC->CIR = 0x00000000;
	// FLASH_Unlock();
	 usart1_deinit();
	 FLASH_If_Init();
	 FLASH_ErasePage(CFG_PAGE_ADDR);

	 if(FLASH_If_Write(&addr,my_fuse,2)==0)printf("\n flash write: OK");
	 printf("\n flash cfg:  flag-0x%x addr-0x%x",*((unsigned long *)CFG_PAGE_ADDR),*((unsigned long *)(CFG_PAGE_ADDR+4)));

	 FLASH_Lock();
	 delay(50);
	 printf("\n\r flash locked!");
     //printf(" OK");
     printf("\n\r WDT reset!");
	 delay(300);
	 IWDG_Enable();
     while(1);
}

unsigned short get_crc(void)
{
  //printf("\n crc_calc:%d",crc_calc);
  return crc_calc;
}

int my_memcmp(unsigned char *data1,unsigned char *data2, int bytes)
{
  int i,ret;
  for(i=0;i<bytes;i++)
  {
    if(data1[i]!=data2[i])return 0;
  }
  return 1;
}

int search_app_version(unsigned long from, unsigned long to, unsigned char *version_data,unsigned long *data_address)
{
  unsigned long addr,i=0;
  unsigned char header[] = "<version_data>";
//  putstr("\n\rsearching_app_version from:0x");
 // print_32(from);
  for(addr=from;addr<to;addr++)
  {
    if(*(unsigned char*)addr == '<')
    {
      if(my_memcmp((unsigned char*)addr,header,14))
      {
        if(data_address!=NULL)*data_address = addr; //todo odkomentiraj
        printf("\n\r version data found!: ");
        addr+=14;
        while(*((unsigned char*)addr) != '<')
        {
          version_data[i++]=*(unsigned char*)(addr++); //todo odkomentiraj
          if(i>=90)break;
        }
        version_data[i]=0; //todo odkomentiraj
        printf("%s",version_data);
        //putstr(version_data);
       return 1; //todo odkomentiraj
      }
    }
  }
 // putstr("\n\r end addr:%x",addr);
  return 0;

}

int cmd_exit_bl(int argc,char **argv)
{
	exit_BL();
	return 0;
}

int cmd_search_appv(int argc,char **argv)
{
	unsigned char app_version_data[100];
	search_app_version(PROGRAM_START_ADDRESS,STM32F_FLASH_ADDR+STM32F_FLASH_SIZE, app_version_data,0);
	return 1;
}



