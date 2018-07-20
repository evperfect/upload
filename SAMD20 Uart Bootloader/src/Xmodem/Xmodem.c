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
** �������� : check
** �������� : У�飬CRCΪ����ΪCRCУ�飬����ΪУ���
** ��ڲ��� : <crc>[in] ѡ����CRCУ�黹��SUMУ��
**			  <buf>[in] У���ԭʼ����
**			  <sz>[in]  У������ݳ���
** ���ڲ��� : ��
** �� �� ֵ : У�����󷵻�TURE,��֮����false
** ����˵�� : ��
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
** �������� : xmodemReceive
** �������� : xmodemЭ����ս����ļ�
** ��ڲ��� : <checkType>[in] �����ļ���У�鷽ʽ��'C':crcУ�飬NAK:�ۼӺ�У��
** ���ڲ��� : ��
** �� �� ֵ : �����ļ�����ʱ����ش������
** ����˵�� : ��
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
	
	//nvm_flash_write((APP_START_ADDRESS-4),nvm_flag,4,1); //���д����ɱ�ʾλ
	while(1)
	{

		//����λ���������ļ�
		for(i=1000;i!=0;i--)
		{
			chTemp = UART_GetByte(100000);
			if(chTemp>0)   break;
			else	UART_PutByte('C');
			
		}
		//�ļ����䳬ʱ
		if((chTemp==0)&&(i==0))
		{
			return TIME_OUT;
		}
		//�û�ȡ���ļ�����
		else if((chTemp=='B')||(chTemp=='b'))
		{
			return USER_CANCELED;
		}
		else
		{ //��ʼ�����ļ�
			
			while(chTemp==SOH)
			{//���յ���Ч����֡ͷ
				xbuff[0]=chTemp;
				for(i=0;i<sizeof(xbuff);i++)
				{   //����һ֡����
					xbuff[i+1] = 0;
					xbuff[i+1]=UART_GetByte(1000);
				}

				if((xbuff[1]==(unsigned char)~xbuff[2])&&(packet_number==xbuff[1])&&(check(1, &xbuff[3], 128)))//����ź�CRC����
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
					UART_PutByte(NAK);//Ҫ���ط�
				}
				do
				{
					chTemp = UART_GetByte(1000);//��ȡ��һ֡���ݵ�֡ͷ
				}while(chTemp==0);
				
			}
			UART_PutByte(chTemp);
			if(chTemp==EOT)
			{//�ļ����ͽ�����־
				UART_PutByte(ACK);
				/*��д���һ֡����*/
				return SUCCESSFULL;
			}
			else if(chTemp==CAN)
			{
				return PARAMETER_ERROR;
			}
			//nvm_flash_read(0x8000,&erase_row_count,1);
			/*�˴����Է��ش�����Ϣ*/
			
		}
	}
} 
 

