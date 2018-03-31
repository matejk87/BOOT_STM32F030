

#ifndef USART1_H_
#define USART1_H_

#include "usart.h"

extern const t_usart usart1;

void usart1_init();
void usart1_deinit(void);

void usart1_putchar(char znak);
char usart1_getchar();
int usart1_rxlen();
int usart1_txlen();
int usart1_txempty();


#endif /* USART1_H_ */
