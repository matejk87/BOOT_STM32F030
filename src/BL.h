#ifndef BL_H_
#define BL_H_

#include "conf_IAP.h"
/*
#include "config.h"

int flash_write(unsigned char *data,int datalen,unsigned long addr); //programsko kodo moramo zaokrožiti na 512B(dopolnimo(padd) s poljubno cifro)
inline unsigned short get_crc(void);
void exit_BL(void);
int flash_erase_to_end(void);
int search_app_version(unsigned long from, unsigned long to, unsigned char *version_data,unsigned long *data_address);

#define FOSC0 16000000
#define FCPU 60000000

#define FRCOSC    AVR32_PM_RCOSC_FREQUENCY
#define OSC0_STARTUP    AVR32_PM_OSCCTRL0_STARTUP_2048_RCOSC

#define BL_485_DATA_FRAME_SIZE 128
#define FLASH_PAGE_SIZE 512

#define EXT_WDT_CLR()   OUT_TOGGLE(WDR_PIN)
*/

extern char version_str[];
extern char HW_version_str[];

int flash_write(unsigned char *data,int datalen,unsigned long addr); //programsko kodo moramo zaokrožiti na 512B(dopolnimo(padd) s poljubno cifro)
int flash_erase_to_end(void);

void exit_BL(void);
unsigned short get_crc(void);
int my_memcmp(unsigned char *data1,unsigned char *data2, int bytes);
int flash_write_last(void);

int search_app_version(unsigned long from, unsigned long to, unsigned char *version_data,unsigned long *data_address);








#endif /*BL_H_*/
