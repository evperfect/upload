/**
 * main.c
 * @kerwin 2015/8/30
 * 
 *
 */

#include <asf.h>
#include "Xmodem.h"
#include "UART.h"
#include "nvm_flash.h"


/**
 * \brief Execute an application from the specified address
 *
 * \param address Application address
 */
void JumpToApplication(uint32_t address)
{
	uint32_t app_start_address;

	/* Rebase the Stack Pointer */
	__set_MSP(*(uint32_t *) address);

	/* Rebase the vector table base address */
	SCB->VTOR = ((uint32_t) address & SCB_VTOR_TBLOFF_Msk);

	/* Load the Reset Handler address of the application */
	app_start_address = *(uint32_t *)(address + 4);

	/* Jump to application Reset Handler in the application */
	asm("bx %0"::"r"(app_start_address));
}

static void check_start_application(void)
{
	
	volatile PortGroup *boot_port = (volatile PortGroup *)(&(PORT->Group[BOOT_LOAD_PIN / 32]));
	volatile bool boot_en;

	/* Enable the input mode in Boot GPIO Pin */
	boot_port->DIRCLR.reg = GPIO_BOOT_PIN_MASK;
	boot_port->PINCFG[BOOT_LOAD_PIN & 0x1F].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	boot_port->OUTSET.reg = GPIO_BOOT_PIN_MASK;
	
	asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");
	asm("nop");asm("nop");asm("nop");

	/* Read the BOOT_LOAD_PIN status */
	boot_en = (boot_port->IN.reg) & GPIO_BOOT_PIN_MASK;

	/* Check the bootloader enable condition */
	if (!boot_en) {
			asm("nop");asm("nop");asm("nop");
			asm("nop");asm("nop");asm("nop");
			asm("nop");asm("nop");asm("nop");

		boot_en = (boot_port->IN.reg) & GPIO_BOOT_PIN_MASK;
		if (!boot_en) 
		{	
		   /* Stay in bootloader */
		   return;
		}
	}
    nvm_flash_read((APP_START_ADDRESS-4),nvm_flag,4);
    if( (nvm_flag[0] == 0xAA) &&(nvm_flag[1] == 0x55) && (nvm_flag[2] == 0xAA) && (nvm_flag[3] == 0x55))
	{
		 JumpToApplication(APP_START_ADDRESS);
	}
   
}
int main (void)
{
	uint8_t status;
	
	 /* Make OSC8M prescalar to zero rather divide by 8 */
	SYSCTRL->OSC8M.bit.PRESC = 0;
    /* Initialize the UART console. */
    usart_open();
	nvm_flash_init();
	check_start_application();
	status = XmodemReceive(APP_START_ADDRESS);
	if(SUCCESSFULL == status )
	{
		nvm_flag[0] = 0xAA;
		nvm_flag[1] = 0x55;
		nvm_flag[2] = 0xAA;
		nvm_flag[3] = 0x55;
		nvm_flash_write((APP_START_ADDRESS-4),nvm_flag,4,1);
		uart_putc("\r\n->Firmware Update Successful ^_^ ^_^ ^_^ £¡\r\n");
		JumpToApplication(APP_START_ADDRESS);
	}	
	uart_putc("\r\n->Firmware Update Failed £¡£¡£¡\r\n");
	while (1);
	
	
}
