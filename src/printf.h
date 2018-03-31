#ifndef PRINTF_H_
#define PRINTF_H_

#define TTY_NULL ((void *)0)
#define TTY0 ((void *)1)

int mprintf(const char *format, ...);
#define printf mprintf

//int print(char **out, const char *format, ...);
int print(char **out, const char *format, ...);
int sprintf(char *out, const char *format, ...);
extern void * print_dev;


#endif /*PRINTF_H_*/


