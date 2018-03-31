
#include "printf.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cmdproc.h"
#include "usart2.h"

#define TTY_NULL ((void *)0)
#define TTY0 ((void *)1)



int getline_on=1;


#define CMD_HIST_LEN 8



static char line[CMD_HIST_LEN][MAX_CMD_LEN]=
{
		{"help"},
		{""},
		{""},
		{""},
		{""},
		{""},
		{""},
		{""}
};
int cnt_hist=0;


#define CNTLQ      0x11
#define CNTLS      0x13
#define DEL        0x7F
#define BACKSPACE  0x08
#define CR         0x0D
#define LF         0x0A
#define ESC        27
#define UP        65
#define DOWN      66

unsigned char cnt_old=0;

#define uart_putchar(a) {usart2_putchar(a); }
#define uart_getchar()  usart2_getchar()
#define uart_rbuflen()  (usart2_rxlen())




void erase_chars(int n){
    for(;n>0;n--)
    {
    	uart_putchar (BACKSPACE);               /* echo backspace                 */
    	uart_putchar (' ');
    	uart_putchar (BACKSPACE);
    }
}

void  getline(void)
{
	static int  cnt = 0;
	static char  esc_cnt=0;
	char c;
	//n=strlen(line[0]);
	if(!uart_rbuflen())return; // ni sprejetega znaka
	c = uart_getchar ();
	if(c==ESC){esc_cnt=1;return;}
	if(esc_cnt){
		if(esc_cnt==1 && c==91) // escape sequenca
		{
			esc_cnt++;
			return;
		}
		if(esc_cnt==2) //obdelava esc sequence
		{
			if(c==UP){
				erase_chars(cnt);
				cnt_hist--;
				cnt_hist&=0x07;
				printf("%s",line[cnt_hist&0x07]);
				cnt=strlen(line[cnt_hist&0x07]);

			}
			if(c==DOWN){
				erase_chars(cnt);
				cnt_hist++;
				cnt_hist&=0x07;
				printf("%s",line[cnt_hist&0x07]);
				cnt=strlen(line[cnt_hist&0x07]);

			}

		}
		esc_cnt=0;
		return;
	}


	if (c == BACKSPACE  ||  c == DEL) {    /* process backspace              */
	      if (cnt)  {
	        cnt--;                             /* decrement count                */
	        uart_putchar (BACKSPACE);               /* echo backspace                 */
	        uart_putchar (' ');
	        uart_putchar (BACKSPACE);
	      }
	      return;
	} // end backspace
	if (c == CNTLQ && c == CNTLS ) return;

	line[cnt_hist][cnt]=c;

	if(c != '\n' && c!='\r')
	{
		uart_putchar (c);                 /* echo and store character       */
	    cnt++;
	}

	if(cnt > MAX_CMD_LEN - 1  ||  c == CR ||   c == LF){         /* check limit and line feed      */
	line[cnt_hist&0x07][cnt] = 0;
	if(cnt>0){
		cmd_proc(line[cnt_hist&0x07]); // prosess command
	cnt_hist++;
	cnt_hist&=0x07;
	}
	printf("\n=>");
	cnt=0;
	}

}

// command function

int bla(int argc,char **argv)
{
	 char cmd[]="osc_lo 1";
	 int n;

	 printf("\nNEW BLA");
	 for(n=0;n<argc;n++){
		 printf("\nArgument%d=%s",n,argv[n]);
	 }
	 printf("\nRUN:%s",cmd);
	 cmd_proc(cmd);
  return 0;
}




/*
int cmd_reset(int argc, char **argv){
	unsigned long tmplong;
	t_timer timedot;
	tmplong=now();
	printf("\nReset");
	while((now()-tmplong)<MAX_RESET_TIME){
		if(timeout(&timedot,1000))printf(" .");
	};
	printf("\nERROR: WDT Disabled -> Reset mcu only");
	ksa_reset_mcu();
	return 1;
}
*/

int cmd_ADDR(int argc, char **argv)
{
	 printf("\nADDR:0");
	 return 0;
}

