#ifndef PROCLINE_H_
#define PROCLINE_H_
#define MAX_CMD_LEN 64


void cmd_proc (const char *cmd);

typedef int (*t_cmd_func)(int argc,char **argv);


typedef struct cmd_str_
  {
	const char *cmdstr;
	t_cmd_func cmd_funct;
	const char *help_str;
  } t_cmd_str;


#endif /* PROCLINE_H_ */
