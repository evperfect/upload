/*
 * Xmodem.c
 *
 * Created: 2015/8/27 22:35:42
 *  Author: kerwinzhong
 */ 
#include "Xmodem.h"
#include "UART.h"
#include "nvm_flash.h"
#include "crc16.h"

volatile uint8_t nvm_flag[4];
uint32_t receive_bytes;
uint16_t  UART_GetByte(uint32_t timeout)
{
	
	uint16_t temp = 0;
    
	while(timeout)
	{
		if(usart_is_rx_ready() )
		{
			temp = uart_read_byte(SERCOM3);
			return (temp);
		}
		else
		{
			timeout--;
		}
	}	
    return (temp);
	
 }
 
 void UART_PutByte(unsigned char c)
 {
	 uart_write_byte(SERCOM3,c);
 }

/**************************************************************************************************
** 函数名称 : check
** 功能描述 : 校验，CRC为真则为CRC校验，否则为校验和
** 入口参数 : <crc>[in] 选择是CRC校验还是SUM校验
**			  <buf>[in] 校验的原始数据
**			  <sz>[in]  校验的数据长度
** 出口参数 : 无
** 返 回 值 : 校验无误返回TURE,反之返回false
** 其他说明 : 无
***************************************************************************************************/
static int check(int crc, const unsigned char *buf, int sz)
{
	if (crc)
	{
		unsigned short crc = crc16_ccitt(buf, sz);
		unsigned short tcrc = (buf[sz]<<8)+buf[sz+1];
		if (crc == tcrc)
		return true;
	}
	else
	{
		int i;
		unsigned char cks = 0;
		for (i = sz; i != 0; i--)
		{
			cks += *(buf++);
		}
		if (cks == *buf)
		return true;
	}
	return false;
}


/**************************************************************************************************
** 函数名称 : xmodemReceive
** 功能描述 : xmodem协议接收接收文件
** 入口参数 : <checkType>[in] 接收文件的校验方式，'C':crc校验，NAK:累加和校验
** 出口参数 : 无
** 返 回 值 : 接收文件操作时的相关错误代码
** 其他说明 : 无
***************************************************************************************************/
unsigned char XmodemReceive(uint32_t address)
{

	uint32_t i=0;	
	unsigned char chTemp=0;
	unsigned char xbuff[133]; /* 128 for XModem	+ 3 head chars + 2 crc + nul */
	unsigned char packet_number = 1;
	uint32_t target_address;
    target_address = address;
    receive_bytes = 0;
	
	//nvm_flash_write((APP_START_ADDRESS-4),nvm_flag,4,1); //清除写入完成标示位
	while(1)
	{

		//向上位机请求发送文件
		for(i=1000;i!=0;i--)
		{
			chTemp = UART_GetByte(100000);
			if(chTemp>0)   break;
			else	UART_PutByte('C');
			
		}
		//文件传输超时
		if((chTemp==0)&&(i==0))
		{
			return TIME_OUT;
		}
		//用户取消文件传输
		else if((chTemp=='B')||(chTemp=='b'))
		{
			return USER_CANCELED;
		}
		else
		{ //开始传输文件
			
			while(chTemp==SOH)
			{//接收到有效数据帧头
				xbuff[0]=chTemp;
				for(i=0;i<sizeof(xbuff);i++)
				{   //接收一帧数据
					xbuff[i+1] = 0;
					xbuff[i+1]=UART_GetByte(1000);
				}

				if((xbuff[1]==(unsigned char)~xbuff[2])&&(packet_number==xbuff[1])&&(check(1, &xbuff[3], 128)))//包序号和CRC无误
				{
						for(i=0;i<128;i++)
						{
							if(xbuff[3+i] != CTRLZ)
							{
								receive_bytes++;
							}
						}
						packet_number++;
						program_memory(target_address,&xbuff[3],128);
						target_address += 128;						
						UART_PutByte(ACK);					    
				}
				else
				{
					UART_PutByte(NAK);//要求重发
				}
				do
				{
					chTemp = UART_GetByte(1000);//读取下一帧数据的帧头
				}while(chTemp==0);
				
			}
			UART_PutByte(chTemp);
			if(chTemp==EOT)
			{//文件发送结束标志
				UART_PutByte(ACK);
				/*烧写最后一帧数据*/
				return SUCCESSFULL;
			}
			else if(chTemp==CAN)
			{
				return PARAMETER_ERROR;
			}
			//nvm_flash_read(0x8000,&erase_row_count,1);
			/*此处可以返回错误信息*/
			
		}
	}
} 
 

