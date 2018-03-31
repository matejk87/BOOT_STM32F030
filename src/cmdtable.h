
#ifndef CMDTABLE_H_
#define CMDTABLE_H_

#include "cmdproc.h"



/* declaration of cmd functions*/

int help(int argc,char **argv);
int bla(int argc,char **argv);
int cmd_exit_bl(int argc,char **argv);
int cmd_reset(int argc,char **argv);
int cmd_search_appv(int argc,char **argv);

//void cmd_enter_bl(int argc,char ** argp);



//int get_inb1(int argc, char ** argp);
/* end of declaration of cmd functions*/
/* CMD table */

// definicija ukazov

const t_cmd_str cmd_table[] =
    {
 /*   "cmd string" , cmd_function,"help string"*/
		{ "help",help,"<-l or cmd> print help"},  // cmdproc.c embedded function
		{ "bla",bla,"test run cmd_process"},
		{ "reset",cmd_reset,"reset"},
		{ "search_appv",cmd_search_appv,"search application version"},
		{"exitBL",cmd_exit_bl,"exit Boot Loader"}

	//	{ "inpb1",get_inb1," get GPIO PB1"},
    };

#endif /* CMDTABLE_H_ */
