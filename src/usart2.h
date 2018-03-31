
#ifndef USART2_H_
#define USART2_H_

#include "usart.h"
extern const t_usart usart2;
void usart2_init();
void usart2_putchar(char znak);
char usart2_getchar();
int usart2_rxlen();
int usart2_txlen();
int usart2_txempty();
#endif /* USART3_H_ */
