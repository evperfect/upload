/*
 * nvm_flash.h
 *
 * Created: 2015/8/25 23:11:30
 *  Author: kerwinzhong
 */ 


#ifndef NVM_FLASH_H_
#define NVM_FLASH_H_

#include "asf.h"

#define MAX_FLASH_ADDR          FLASH_SIZE
#define FLASH_ROW_SIZE          (FLASH_PAGE_SIZE*4)
#define FLASH_NB_OF_ROWS        (FLASH_NB_OF_PAGES/4)

#define TEST_ADDR                0x(FLASH_ADDR+((FLASH_NB_OF_PAGES-1)*FLASH_PAGE_SIZE))

void configure_nvm(void);
void nvm_flash_init(void);
uint8_t  nvm_flash_write( uint32_t target_address,uint8_t *DateBuffer,uint8_t len,uint8_t backup);
uint8_t  nvm_flash_read( uint32_t target_address,uint8_t *DateBuffer,uint8_t len);
void program_memory(uint32_t address, uint8_t *buffer, uint16_t len);


#endif /* NVM_FLASH_H_ */