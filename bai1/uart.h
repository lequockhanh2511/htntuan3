#ifndef UART_H
#define UART_H

#include <stdint.h>

void UART1_Init(void);
void UART1_SendChar(char c);
void UART1_SendString(const char *str);
int UART1_ReadChar(char *c);

#endif
