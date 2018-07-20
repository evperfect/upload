/*
 * Xmodem.h
 *
 * Created: 2015/8/27 22:36:03
 *  Author: kerwinzhong
 */ 


#ifndef XMODEM_H_
#define XMODEM_H_

#include "asf.h"

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define SUCCESSFULL     0X00
#define TIME_OUT        0X01
#define USER_CANCELED   0X02
#define PARAMETER_ERROR 0X03

extern volatile uint8_t nvm_flag[4];
extern uint32_t receive_bytes;
unsigned char XmodemReceive(uint32_t address);
uint16_t  UART_GetByte(uint32_t timeout);
 void UART_PutByte(unsigned char c);
#endif /* XMODEM_H_ */