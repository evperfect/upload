/*
 * nvm_flash.c
 *
 * Created: 2015/8/25 23:12:05
 *  Author: kerwinzhong
 */ 
#include "nvm_flash.h"


void configure_nvm(void)
{
	struct nvm_config config_nvm;
	nvm_get_config_defaults(&config_nvm);
	/* Enable automatic page write mode */
	config_nvm.manual_page_write = false;
	nvm_set_config(&config_nvm);
}

void nvm_flash_init(void)
{
	configure_nvm();
}

uint8_t  nvm_flash_write( uint32_t target_address,uint8_t *DateBuffer,uint8_t len,uint8_t backup)
{
    uint16_t row_number;
    uint32_t  row_address;
    uint8_t temp_buffer[FLASH_ROW_SIZE],offset;

    
    if (target_address > MAX_FLASH_ADDR)
    {
        return 1;
    }
    row_number = target_address / FLASH_ROW_SIZE;
    if(row_number > FLASH_NB_OF_ROWS)
    {
        return 2;
    }  
    if ( len > FLASH_ROW_SIZE )
    {
       return 3;
    }
    row_address = row_number * FLASH_ROW_SIZE;
	offset = target_address % FLASH_ROW_SIZE;
	if(backup)
	{
		nvm_read_buffer(row_address,&temp_buffer[0],FLASH_PAGE_SIZE);
		nvm_read_buffer(row_address+FLASH_PAGE_SIZE,&temp_buffer[FLASH_PAGE_SIZE],FLASH_PAGE_SIZE);
		nvm_read_buffer(row_address+FLASH_PAGE_SIZE*2,&temp_buffer[FLASH_PAGE_SIZE*2],FLASH_PAGE_SIZE);
		nvm_read_buffer(row_address+FLASH_PAGE_SIZE*3,&temp_buffer[FLASH_PAGE_SIZE*3],FLASH_PAGE_SIZE);
	}  
	
    for(uint8_t i=0;i<len;i++)
    {
        temp_buffer[i+offset] = DateBuffer[i];
    }
    nvm_erase_row(row_address);
	nvm_write_buffer(row_address,temp_buffer,FLASH_PAGE_SIZE);
    nvm_write_buffer(row_address+FLASH_PAGE_SIZE,&temp_buffer[FLASH_PAGE_SIZE],FLASH_PAGE_SIZE);
    nvm_write_buffer(row_address+FLASH_PAGE_SIZE*2,&temp_buffer[FLASH_PAGE_SIZE*2],FLASH_PAGE_SIZE);
    nvm_write_buffer(row_address+FLASH_PAGE_SIZE*3,&temp_buffer[FLASH_PAGE_SIZE*3],FLASH_PAGE_SIZE);
    return 0;
}

uint8_t  nvm_flash_read( uint32_t target_address,uint8_t *DateBuffer,uint8_t len)
{
	uint16_t  row_number;
	uint32_t  row_address;
	uint8_t temp_buffer[FLASH_ROW_SIZE],offset;

	
	if (target_address > MAX_FLASH_ADDR)
	{
		return 1;
	}
	row_number = target_address / FLASH_ROW_SIZE;
	if(row_number > FLASH_NB_OF_ROWS)
	{
		return 2;
	}
	if ( len > FLASH_ROW_SIZE )
	{
		return 3;
	}
	row_address = row_number * FLASH_ROW_SIZE;
	offset = target_address % FLASH_ROW_SIZE;
	nvm_read_buffer(row_address,temp_buffer,FLASH_PAGE_SIZE);
	nvm_read_buffer(row_address+FLASH_PAGE_SIZE,&temp_buffer[FLASH_PAGE_SIZE],FLASH_PAGE_SIZE);
	nvm_read_buffer(row_address+FLASH_PAGE_SIZE*2,&temp_buffer[FLASH_PAGE_SIZE*2],FLASH_PAGE_SIZE);
	nvm_read_buffer(row_address+FLASH_PAGE_SIZE*3,&temp_buffer[FLASH_PAGE_SIZE*3],FLASH_PAGE_SIZE);
	for(uint8_t i=0;i<len;i++)
	{
		DateBuffer[i] = temp_buffer[offset+i];
	}
	return 0;
}

/**
 * \brief Function for programming data to Flash
 *
 * This function will check whether the data is greater than Flash page size.
 * If it is greater, it splits and writes pagewise.
 *
 * \param address address of the Flash page to be programmed
 * \param buffer  pointer to the buffer containing data to be programmed
 * \param len     length of the data to be programmed to Flash
 */
void program_memory(uint32_t address, uint8_t *buffer, uint16_t len)
{
	/* Check if length is greater than Flash page size */
	if (len > NVMCTRL_PAGE_SIZE) {
		uint32_t offset = 0;

		while (len > NVMCTRL_PAGE_SIZE) {
			/* Check if it is first page of a row */
			if ((address & 0xFF) == 0) {
				/* Erase row */
				nvm_erase_row(address);
			}
			/* Write one page data to flash */
			nvm_write_buffer(address, buffer + offset, NVMCTRL_PAGE_SIZE);
			/* Increment the address to be programmed */
			address += NVMCTRL_PAGE_SIZE;
			/* Increment the offset of the buffer containing data */
			offset += NVMCTRL_PAGE_SIZE;
			/* Decrement the length */
			len -= NVMCTRL_PAGE_SIZE;
		}

		/* Check if there is data remaining to be programmed */
		if (len > 0) {
			/* Write the data to flash */
			nvm_write_buffer(address, buffer + offset, len);
		}
	} else {
		/* Check if it is first page of a row) */
		if ((address & 0xFF) == 0) {
			/* Erase row */
			nvm_erase_row(address);
		}
		/* Write the data to flash */
		nvm_write_buffer(address, buffer, len);
	}
}