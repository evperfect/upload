/*
 * UART.h
 *
 * Created: 2015/9/3 18:29:42
 *  Author: kerwinzhong
 */ 


#ifndef UART_H_
#define UART_H_

#include "asf.h"

void uart_write_byte(Sercom *sercom, uint8_t data);
uint8_t uart_read_byte(Sercom *sercom);
void usart_open();
bool usart_is_rx_ready(void) ;
void uart_putc(char *str);
#endif /* UART_H_ */