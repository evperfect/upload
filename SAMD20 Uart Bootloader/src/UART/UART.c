/*
 * UART.c
 *
 * Created: 2015/9/3 18:29:30
 *  Author: kerwinzhong
 */ 
#include "UART.h"

// UART setup , 115200,8,N,1
void usart_open()
{
	/* Enable & configure alternate function C for pins PA24 & PA25 */
	PORT->Group[0].PINCFG[24].bit.PMUXEN = 1;
	PORT->Group[0].PINCFG[25].bit.PMUXEN = 1;
	PORT->Group[0].PMUX[12].reg = 0x22;
	/* Enable APB clock for SERCOM3 */
	PM->APBCMASK.reg |= (1u << 5);
	/* Configure GCLK generator 0 as clock source for SERCOM3 */
	GCLK->CLKCTRL.reg = 0x4010;
	/* Configure SERCOM3 USART with baud 115200 8-N-1 settings */
	SERCOM3->USART.CTRLA.reg = SERCOM_USART_CTRLA_RXPO(3) | SERCOM_USART_CTRLA_TXPO | SERCOM_USART_CTRLA_MODE(1) | SERCOM_USART_CTRLA_DORD;
	while(SERCOM3->USART.STATUS.bit.SYNCBUSY);
	SERCOM3->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0);
	SERCOM3->USART.BAUD.reg = 50436;
	while(SERCOM3->USART.STATUS.bit.SYNCBUSY);
	SERCOM3->USART.CTRLA.reg |= 0x02; // uart enable
}

void uart_write_byte(Sercom *sercom, uint8_t data)
{
	while(!sercom->USART.INTFLAG.bit.DRE);
	sercom->USART.DATA.reg = (uint16_t)data;
}
uint8_t uart_read_byte(Sercom *sercom)
{
	while(!sercom->USART.INTFLAG.bit.RXC);
	return((uint8_t)(sercom->USART.DATA.reg & 0x00FF));
}
bool usart_is_rx_ready(void) 
{
	return (SERCOM3->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXC);
}

void uart_putc(char *str)
{
	while(*str != NULL)
	{
		uart_write_byte(SERCOM3,*str);
		str++;
	}
}