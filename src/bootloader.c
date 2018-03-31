/*
 * bootloader.c
 *
 *  Created on: 15. apr. 2013
 *      Author: Matej
 */
#include "bootloader.h"
#include "conf_IAP.h"
#include "flash_if.h"
#include "printf.h"
#include "stm32f0xx_iwdg.h"


void copy_vector_table_sram(void)
{
    // Copy interrupt vector table to the RAM.
    volatile uint32_t *VectorTable = (volatile uint32_t *)0x20000000;
    uint32_t ui32_VectorIndex = 0;

    for(ui32_VectorIndex = 0; ui32_VectorIndex < 48; ui32_VectorIndex++)
    {
        VectorTable[ui32_VectorIndex] = *(__IO uint32_t*)((uint32_t)PROGRAM_START_ADDRESS + (ui32_VectorIndex << 2));
    }
 /*   __HAL_RCC_AHB_FORCE_RESET();

    //  Enable SYSCFG peripheral clock
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    __HAL_RCC_AHB_RELEASE_RESET();

    // Remap RAM into 0x0000 0000
    __HAL_SYSCFG_REMAPMEMORY_SRAM();*/
    /* Enable the SYSCFG peripheral clock*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Remap SRAM at 0x00000000 */
    SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
}


void resetcpu(void)
{
    //volatile int i;
    printf("\nreset mcu:(internal WD)");
    IWDG_Enable();
    while(1){}
}

int write_RS485addr_to_flash(unsigned short rs485_addr)
{
	unsigned long my_fuse[2];
	unsigned long addr=CFG_PAGE_ADDR;
	my_fuse[0]=0xffffffff;
	my_fuse[1]=rs485_addr;
	FLASH_Unlock();
	FLASH_If_Init();
	if (FLASH_ErasePage(CFG_PAGE_ADDR) != FLASH_COMPLETE) return 0;
	if(FLASH_If_Write(&addr,my_fuse,2)==0)
	{
		printf("\n flash write: OK");
		return 1;
	}
	else
	{
		printf("\n flash write: ERROR");
		return 0;
	}
	return 0;
}


unsigned int get_RS485addr_from_flash(void)
{
	unsigned long *ptr= ((unsigned long *)(CFG_PAGE_ADDR+4));
	printf("\n rs485 address in flash:0x%x",*ptr);
	return *ptr;
}

void enter_bootloader(unsigned short rs485_addr)
{
    printf("todo: enter bootloader!");
	unsigned long my_fuse[2];
	my_fuse[0]=0;
	my_fuse[1]=rs485_addr;
	unsigned long addr=CFG_PAGE_ADDR;
	printf("\n going to bootloader! RS485 addr:0x%x",rs485_addr);
	FLASH_Unlock();
	FLASH_If_Init();
	FLASH_ErasePage(CFG_PAGE_ADDR);
	 if(FLASH_If_Write(&addr,my_fuse,2)==0)printf("\n flash write: OK");
	 printf("\n bootloader flag: 0x%x 0x%x",*((unsigned long *)CFG_PAGE_ADDR),*((unsigned long *)(CFG_PAGE_ADDR+4)));
	 FLASH_Lock();
	 resetcpu();
}

int cmd_set_rs485_addr(int argc, char **argv)
{
	unsigned short addr=0;
	unsigned long *ptr= ((unsigned long *)(CFG_PAGE_ADDR+4));
	addr = strtoul(argv[1],0,0);
	if(argc>1)
	{
		if(write_RS485addr_to_flash(addr))
		{

		}
		else
		{

		}
	}
	else
	{

	}
	printf("\n rs485 address in flash:0x%x",*ptr);
	return 1;
}
