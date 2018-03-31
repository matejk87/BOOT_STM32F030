
#include "cmdproc.h"
#include "printf.h"
#include "cmdtable.h"

static unsigned char cmd_search ( int argc,char **argv);
#define CMD_TABLE_LEN (sizeof (cmd_table) / sizeof (cmd_table [0]))

void cmd_proc (const char *cmd)
{
char cmdstr_buf [1 + MAX_CMD_LEN];
char * arg_p_buf [1 + MAX_CMD_LEN/2];
int argp_cnt=1;
int n;
strncpy (cmdstr_buf, cmd, sizeof (cmdstr_buf) - 1);


arg_p_buf[0]=cmdstr_buf;
for(n=0;n<MAX_CMD_LEN;n++){
	if(cmdstr_buf[n]==0)break;
	if(cmdstr_buf[n]==' '){
		cmdstr_buf[n]=0;
		while(cmdstr_buf[n+1]==' '){
			n++;
		};
		if(cmdstr_buf[n+1]!=0)arg_p_buf[argp_cnt++]=&cmdstr_buf[n+1];
	}
}
cmd_search(argp_cnt,arg_p_buf);
return;
}


static unsigned char cmd_search ( int argc,char **argv){

t_cmd_str *ctp;
char *cmdstr=argv[0];
for (ctp = (t_cmd_str *)cmd_table; ctp < &cmd_table[CMD_TABLE_LEN]; ctp++)
  {
	  if (strcmp (ctp->cmdstr, cmdstr) == 0){
	   (ctp->cmd_funct)(argc,argv);
		return 0;

	  }
  }
  printf("\nError:Unknown Command");
  return 1;
}


int help(int argc,char **argv)
{
	t_cmd_str *ctp;
	printf("\nHelp\n");

	  if(argc>1)
	  {
		  if(argv[1][0]=='-'){
			  if(argv[1][1]=='l'){
				  printf("\nCmd list:");
				  for (ctp = (t_cmd_str *)cmd_table; ctp < &cmd_table[CMD_TABLE_LEN-1]; ctp++)
				  {
					printf(" %s,",ctp->cmdstr);
				  }
				  printf(" %s",ctp->cmdstr);
			  }else printf("\nUnknown parameter");
		  }else{
			  for (ctp = (t_cmd_str *)cmd_table; ctp < &cmd_table[CMD_TABLE_LEN]; ctp++)
			  {
				  if(!strcmp(ctp->cmdstr,argv[1])){
					  printf("\n  %-15s %s",ctp->cmdstr,ctp->help_str);
					  break;
				  }
			  }
			  printf("\nUnknown command");

		  }
	  }
	  else
	  {
		  for (ctp = (t_cmd_str *)cmd_table; ctp < &cmd_table[CMD_TABLE_LEN]; ctp++)
			{

			  printf("\n  %12s %s",ctp->cmdstr,ctp->help_str);
			}
	  }
	  return 0;
}

