#include "mybus.h"
#include "crc16.h"
#include "timer.h"
#include "conf_IAP.h"
#include "config.h"
#include "usart1.h"
#include "printf.h"
#include "bootloader.h"
#include "BL.h"
//#include <string.h>


#define PRINT_TX 0

// common serial functions
#define MB_PING_FUNCT 0x00
#define MB_ACK_FUNCT 0x00
#define MB_NACK_FUNCT 0x01
#define MB_UPTIME_FUNCT 0x7
#define MB_RESET_CPU 0xB
#define MB_SET_ADDR 0x7d
#define MB_CLEAR_RESET_FLAG_FUNCT 0x7e
#define MB_GET_VERSION_FUNCT 0x7f
//#define GET_STATUS_FUNCT                2

// special serial functions
/*#define MB_SET_ROLETA 0x5
#define MB_GET_ROLETA 0x6
#define MB_SET_ROL_PAR 0x8
#define MB_GET_ROL_PAR 0x9*/


#define MYBUS_RX_BUFF_SIZE 256
#define MYBUS_TX_BUFF_SIZE 256

t_mybus rx;
t_mybus tx;

t_timer stx_timer;
t_timer stx_rx_timer;
t_timer stx_act_led_timer;
t_timer timerreadmodaddr;
t_timer addr_timer;

#define RESET_FLAG_MASK 0x01

#define MOD_DEFAULT_ADDR  0x0001

volatile unsigned char reset_flag = 0x01;

#define mFlags  (reset_flag&RESET_FLAG_MASK)

unsigned char app_version_data[100];

//const char ver_opis[]={VER_OPIS};

//extern t_timer timer_modul_reset;

int printchr(char c)
{
	usart1_putchar(c);
  return 0;
}

unsigned short mod_addr = 0x3100;//BL_485_ADDR;

unsigned char rx_buffer[MYBUS_RX_BUFF_SIZE];
unsigned char tx_buffer[MYBUS_TX_BUFF_SIZE];
unsigned long tmp32,cfg_ok,time_from_start;

int mybus_st_init(t_mybus * dev, unsigned char * data, int len)
{
  dev->state = MYBUS_STX;
  dev->maxlen = 0;
  dev->data = data;
  dev->maxlen = len;
  dev->cnt = 0;
  return 0;
}

int mybus_put_long(unsigned char * tx_data, unsigned long data)
{
  int tx_len = 0;
  tx_data[tx_len++] = data & 0xff;
  tx_data[tx_len++] = (data >> 8) & 0xff;
  tx_data[tx_len++] = (data >> 16) & 0xff;
  tx_data[tx_len++] = (data >> 24) & 0xff;
  return tx_len;
}

int mybus_put_short(unsigned char * tx_data, unsigned short data)
{
  int tx_len = 0;
  tx_data[tx_len++] = data & 0xff;
  tx_data[tx_len++] = (data >> 8) & 0xff;
  return tx_len;
}

int mybus_put_char(unsigned char * tx_data, unsigned char data)
{
  int tx_len = 0;
  tx_data[tx_len++] = data & 0xff;
  return tx_len;
}

#define CHAR  1
#define SHORT 2
#define LONG  4

unsigned long mybus_get(unsigned char * rx_data,int type)
{
  unsigned long tmp=0;
  tmp = rx_data[0];
  if(type == CHAR)return tmp;
  tmp |= (rx_data[1]<<8);
  if(type == SHORT)return tmp;
  tmp |= (rx_data[2]<<16);
  tmp |= (rx_data[3]<<24);
  return tmp;
}

int
tx_data(t_mybus * dev, int
(*fputchar)(char))
{
  unsigned char cval;
  int n;
  fputchar(STX);
#if PRINT_TX
  printf("\nTX:%02x",STX);
#endif

  dev->crc = 0;

  cval = dev->src & 0xFF;
  crc16_n(&(dev->crc), (char *) &cval, 1);
  fputchar(cval);
#if PRINT_TX
  printf(" %02x",cval);
#endif

  cval = (dev->src >> 8) & 0xFF;
  crc16_n(&(dev->crc), (char *) &cval, 1);
  fputchar(cval);
#if PRINT_TX
  printf(" %02x",cval);
#endif

  cval = dev->dest & 0xFF;
  crc16_n(&(dev->crc), (char *) &cval, 1);
  fputchar(cval);
#if PRINT_TX
  printf(" %02x",cval);
#endif

  cval = (dev->dest >> 8) & 0xFF;
  crc16_n(&(dev->crc), (char *) &cval, 1);
  fputchar(cval);
#if PRINT_TX
  printf(" %02x",cval);
#endif

  fputchar(dev->msg_number);
  crc16_n(&(dev->crc), (char *) &dev->msg_number, 1);
#if PRINT_TX
  printf(" %02x",dev->msg_number);
#endif

  fputchar(dev->funct);
  crc16_n(&(dev->crc), (char *) &dev->funct, 1);
#if PRINT_TX
  printf(" %02x",dev->funct);
#endif

  fputchar(dev->flags);
#if PRINT_TX
  printf(" %02x",dev->flags);
#endif

  crc16_n(&(dev->crc), (char *) &dev->flags, 1);
  fputchar(dev->datalen);
#if PRINT_TX
  printf(" %02x",dev->datalen);
#endif

  crc16_n(&(dev->crc), (char *) &dev->datalen, 1);
  for (n = 0; n < (dev->datalen); n++)
    {
      fputchar(dev->data[n]);
#if PRINT_TX
      printf(" %02x",dev->data[n]);
#endif

      crc16_n(&(dev->crc), (char *) &dev->data[n], 1);
    }
  fputchar(dev->crc & 0xff);
#if PRINT_TX
  printf(" %02x",dev->crc & 0xff);
#endif

  fputchar((dev->crc >> 8) & 0xff);
#if PRINT_TX
  printf(" %02x",(dev->crc >> 8) & 0xff);
#endif

  fputchar(ETX);
#if PRINT_TX
  printf(" %02x",ETX);
#endif
  dev->stats.tx_msg_cnt++; // update stats
  return 0;
}

void mybus_init(void)
{
  printf("\n mybus init");
  printf("\n usart1 init");
  usart1_init();
  printf(":OK");
 // mod_addr = (*((unsigned long *)(MYISP_FB_ADDR+4)))&0xffff;//BL_485_ADDR;
/*  if(get_RS485addr_from_flash() >= 0xffff)
  {
	  //printf("\n\rwriting default addr to flash");
	  write_RS485addr_to_flash(MOD_DEFAULT_ADDR);
  }*/
  mod_addr=get_RS485addr_from_flash();
  printf("\n\rRS485 addr:0x%x",mod_addr);
  // printf("\nmod addr:0x%04x", mod_addr);
  mybus_st_init(&rx, rx_buffer, MYBUS_RX_BUFF_SIZE);
  mybus_st_init(&tx, tx_buffer, MYBUS_TX_BUFF_SIZE);

}

void mybus_make_tx_header(t_mybus * rxdev, t_mybus * txdev)
{
  txdev->src = mod_addr;
  txdev->dest = rxdev->src;
  txdev->flags = mFlags;
  txdev->msg_number = rxdev->msg_number;
  //printf("\n txdev->src:%x",txdev->src);
}

void mybus_main(void)
{
  char rxchar = 0;
  static unsigned long tmp=0;
  int j;
  if (rx.state != MYBUS_END)
    {
      if (usart1_rxlen())
        {
          rxchar = usart1_getchar();
         // usart2_putchar(rxchar); //todo odstrani
         // printf("0x%x ",rxchar); //todo odstrani
          switch (rx.state)
            {
          case MYBUS_STX:
            timeout(&stx_rx_timer, 0);
            time_from_start = timetick;
            if (rxchar == 0x55)
              rx.state = MYBUS_SRC0;
             printf("\n");
            break;
          case MYBUS_SRC0:
          //  printf("\nSRC0:%02x",rxchar);
            rx.crc_calc = 0;
            rx.src = rxchar & 0x00FF;
            crc16_n(&(rx.crc_calc), &rxchar, 1);
            rx.state = MYBUS_SRC1;
            break;
          case MYBUS_SRC1:
          //  printf("\nSRC1:%02x",rxchar);
            rx.src |= (rxchar << 8) & 0xFF00;
            crc16_n(&(rx.crc_calc), &rxchar, 1);
            rx.state = MYBUS_DEST0;
            break;
          case MYBUS_DEST0:
         //   printf("\nDEST0:%02x",rxchar);
            rx.state = MYBUS_DEST1;
            crc16_n(&(rx.crc_calc), &rxchar, 1);
            rx.dest = rxchar & 0x00FF;
            break;
          case MYBUS_DEST1:
         //   printf("\nDEST1:%02x",rxchar);
            rx.state = MYBUS_MSG_NUMBER;
            crc16_n(&(rx.crc_calc), &rxchar, 1);
            //   printf("dest:%04x",rx.dest );
            rx.dest |= (rxchar << 8) & 0xFF00;
            break;
          case MYBUS_MSG_NUMBER:
            rx.state = MYBUS_FUNCT;
            crc16_n(&(rx.crc_calc), &rxchar, 1);
            rx.msg_number = rxchar;
            break;
          case MYBUS_FUNCT:
            rx.state = MYBUS_FLAGS;
            crc16_n(&(rx.crc_calc), &rxchar, 1);
            rx.funct = rxchar;
            break;
          case MYBUS_FLAGS:
           // printf("\nFLAGS0:%02x",rxchar);
            rx.state = MYBUS_DATALEN;
            crc16_n(&(rx.crc_calc), &rxchar, 1);
            rx.flags = rxchar;
            break;
          case MYBUS_DATALEN:
           // printf("\nDLEN:%02x",rxchar);
            crc16_n(&(rx.crc_calc), &rxchar, 1);
            rx.datalen = rxchar;
            rx.cnt = 0;
            if (rx.datalen)
              rx.state = MYBUS_DATA;
            else
              rx.state = MYBUS_CRC1;
            break;
          case MYBUS_DATA:
            rx.data[rx.cnt] = rxchar;
            crc16_n(&(rx.crc_calc),
                (char *) &(rx.data[rx.cnt]), 1);
            rx.cnt++;
            if (rx.cnt == rx.datalen)
              {
                rx.state = MYBUS_CRC1;
              }
            break;
          case MYBUS_CRC1:
            rx.state = MYBUS_CRC2;
            rx.crc = rxchar & 0xFFUL;
            break;
          case MYBUS_CRC2:
            rx.state = MYBUS_ETX;
            rx.crc |= (rxchar & 0xFFUL) << 8;
            break;
          case MYBUS_ETX:
            if (rxchar == ETX)
              { // ali je etx ok
                if (rx.crc_calc == rx.crc)
                  { // preveri crc
                    rx.state = MYBUS_END;
                    rx.stats.rx_total_msg_cnt++; // update stats
                    timeout(&stx_timer,0);
                    //printf("\nRX OK");
                    // return 1;
                  }
                else // CRC ERROR
                  {
                    rx.stats.rx_crc_err_cnt++; // update stats
                    rx.state = MYBUS_STX;
                    printf("\n crc_error");
                   // printf(" sr:%04x de:%04x", rx.src, rx.dest);
                    //return -2;
                  }
              }
            else // etx error
              {
                rx.stats.rx_etx_err_cnt++; // update stats
                rx.state = MYBUS_STX;
                printf("\n etx_error");
                // return -1;
              }
            break;
            }
        }
      else if ((rx.state != MYBUS_STX) && timeout(&stx_rx_timer,1000))
        {
    	  printf("\n\rtimeout");
          rx.state = MYBUS_STX;
          rx.stats.rx_timeout_cnt++;

        }
    }
  else
    {
     // printf("\nrxok src:%04x dest:%04x",rx.src,rx.dest);
      if ((rx.dest == mod_addr) && (rx.src != mod_addr))
        {
          if (!timeout(&stx_timer,5))
            return;
          rx.stats.rx_msg_cnt++;
         // printf("\nrxok dest:%04x",rx.dest);
          //  printf("\nrx:%02x",rx.funct);
          timeout(&stx_act_led_timer, 0);
          //timeout(&timer_modul_reset, 0);
         // tmr_reset_timer();
          mybus_make_tx_header(&rx, &tx);
          switch (rx.funct)
            {
          case MB_PING_FUNCT:
           // printf("\nping/ack");
            tx.funct = MB_ACK_FUNCT;
            tx.datalen = 0;

            tx_data(&tx, printchr);
            break;
          case MB_GET_VERSION_FUNCT:
              tx.funct = MB_GET_VERSION_FUNCT; //todo C# verzija bootloaderja ne dela s tem sporoèilom!
              tx.datalen = 0;
              for(j=0;j<strlen(VER_OPIS);j++)
                {
                         tx.datalen += mybus_put_char(&tx.data[tx.datalen], VER_OPIS[j]); // opis
                }
              tx.datalen += mybus_put_char(&tx.data[tx.datalen], 0);
              tx_data(&tx, printchr);

         /*   tx.funct = MB_GET_VERSION_FUNCT; // old version of message
            tx.datalen = 0;
            tx.datalen += mybus_put_short(&tx.data[tx.datalen], 0); // oznaka modula
            tx.datalen += mybus_put_char(&tx.data[tx.datalen], 0); //driver
            tx.datalen += mybus_put_char(&tx.data[tx.datalen], 0); //aplikacijske variante 220V modul
            tx.datalen += mybus_put_char(&tx.data[tx.datalen], 0); //interne izbolšave
            tx.datalen += mybus_put_char(&tx.data[tx.datalen], 0); //tiv
            tx.datalen += mybus_put_char(&tx.data[tx.datalen], 0); // sestav
            tx.datalen += mybus_put_char(&tx.data[tx.datalen], VER_HW); // hw
            tx.datalen += mybus_put_char(&tx.data[tx.datalen],strlen(VER_OPIS) ); //len string
            for(j=0;j<strlen(VER_OPIS);j++)
                {
                  tx.datalen += mybus_put_char(
                      &tx.data[tx.datalen], VER_OPIS[j]); // opis
                }
                tx_data(&tx, printchr);
                break;
*/

          case MB_PING_BL:
            tx.funct = MB_ACK_FUNCT;
            tx.datalen = 0;
            printf("\n ping_BL");
            tx_data(&tx, printchr);
            break;

       /*   case GET_STATUS_FUNCT:
            stxetx_tx.funct = GET_STATUS_FUNCT;
            stxetx_tx.datalen = 0;

            stxetx_tx_data(&stxetx_tx, printchr);
            break;*/

          case MB_GET_APP_VERSION:
            tx.funct = MB_GET_APP_VERSION;
            tx.datalen = 0;

            if(search_app_version( PROGRAM_START_ADDRESS,STM32F_FLASH_ADDR+STM32F_FLASH_SIZE, app_version_data,0))
            {
              app_version_data[99]=0;//varovalka
              tx.data = app_version_data;
              while(tx.data[tx.datalen]!=0)
              {
                tx.datalen++;
              }
              tx.datalen++;//dodfamo se nièlo za konec stringa
            }
            else tx.datalen=0;
            //stxetx_tx.datalen = strlen((char *)stxetx_tx.data)+1;

            tx_data(&tx, printchr);
            break;
          case MB_PROGRAM_DATA:
            tx.datalen = 0;
            tmp = mybus_get(&rx.data[128],sizeof(unsigned long));
            //putstr("\n\r program_data addr:0x");
          //  print_32(tmp);
            if(flash_write(rx.data,128,mybus_get(&rx.data[128],sizeof(unsigned long))))tx.funct = MB_PROGRAM_DATA;
            else tx.funct = MB_NACK_FUNCT;

            tx_data(&tx, printchr);
            break;
          case MB_EXIT_BL:
        	//putstr("\n\rcmd:EXIT_BL");  //todo
            tx.funct = MB_EXIT_BL;
            tx.datalen = 0;
            tx_data(&tx, printchr);
            timeout(&addr_timer,0);
            while(!timeout(&addr_timer,50));
            //flash_erase_to_end(); todo preveri
            exit_BL();
            break;
          case MB_GET_CRC:
        	//printf("\n MB_GET_CRC");
            tx.funct = MB_GET_CRC;
            tx.datalen = 0;
            tx.datalen += mybus_put_short( &tx.data[tx.datalen],get_crc());
            tx_data(&tx, printchr);
            delay(15);
            flash_write_last(); //todo preveri
            break;
          case MB_GET_APP_OFFSET:
            tx.funct = MB_GET_APP_OFFSET;
            tx.datalen = 0;
            tx.datalen += mybus_put_long( &tx.data[tx.datalen],PROGRAM_START_OFFSET);
            tx_data(&tx, printchr);
            break;
       /*   case UPTIME_FUNCT:
            stxetx_tx.funct = UPTIME_FUNCT;
            stxetx_tx.datalen = 0;
            stxetx_tx.datalen += stxetx_put_long( &stxetx_tx.data[stxetx_tx.datalen], uptime);
            stxetx_tx_data(&stxetx_tx, printchr);
            break;*/



          case MB_UPTIME_FUNCT:
            tx.funct = MB_UPTIME_FUNCT;
            tx.datalen = 0;
            tx.datalen += mybus_put_long( &tx.data[tx.datalen], timesec);
            tx_data(&tx, printchr);
            break;
          case MB_CLEAR_RESET_FLAG_FUNCT:
            reset_flag = 0x00;
            tx.flags = mFlags;
            tx.funct = MB_ACK_FUNCT;
            tx.datalen = 0;
            tx_data(&tx, printchr);
            break;

          default:
            tx.funct = 0x01; // nack
            tx.datalen = 0;
            printf("nack! unknown command:0x%x",rx.funct);
            tx_data(&tx, printchr);
            break;
            }
          rx.state = MYBUS_STX; // rx complette

        } //addr ok
      else
        {
          rx.state = MYBUS_STX;
        }
    }// end // RX OK
}

int cmd_stats(int argc, char **argv)
{
  printf("\nmybus statistics");
  printf("\nAddr:%02x",/*MODUL_DEFAULT_ADDR+*/mod_addr);
  printf("\nrx_tot_msg_cnt:%d", rx.stats.rx_total_msg_cnt);
  printf("\nrx_msg_cnt    :%d", rx.stats.rx_msg_cnt);
  printf("\nrx_etx_err_cnt:%d", rx.stats.rx_etx_err_cnt);
  printf("\nrx_timeout_cnt:%d", rx.stats.rx_timeout_cnt);
  printf("\nrx_crc_err_cnt:%d", rx.stats.rx_crc_err_cnt);
  printf("\nrx_etx_err_cnt:%d", rx.stats.rx_etx_err_cnt);
  return 0;
}

