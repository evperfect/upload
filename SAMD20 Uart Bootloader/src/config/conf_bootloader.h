/**
 * \file
 *
 */

#ifndef CONF_BOOTLOADER_H
#define CONF_BOOTLOADER_H


#define APP_START_ADDRESS          0xC00  //3K for bootloader
#define BOOT_LOAD_PIN              PIN_PA15
#define GPIO_BOOT_PIN_MASK         (1U << (BOOT_LOAD_PIN & 0x1F))

#endif // CONF_BOOTLOADER_H
