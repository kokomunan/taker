#ifndef __EXT_FLASH_H_
#define __EXT_FLASH_H_
#include <stdint.h>



#define FLASH_MID  0xC8
#define FLASH_DID  0x15

#define FLASH_MAX_SECTOR_NUM   1024

#define FLASH_SECTOR_SIZE   0x1000
//flash cmd
#define FLASH_WRITE_ENABLE  0x06
#define FLASH_READ_STATUS   0x05

#define FLASH_READ_DATA     0x03
#define FLASH_WRITE_PAGE    0x02
#define FLASH_SECTOR_ERASE  0x20
#define FLASH_READ_ID       0x90
#define FLASH_DEEP_POWER_DOWN       0xB9
#define FLASH_WAKE_UP               0xAB



uint8_t flash_hw_check(void);
uint8_t flash_status_check(void);
void flash_write_enable(void);
void flash_erase_sector(uint16_t sector_num);
uint32_t get_free_sector(void);
 uint32_t get_next_sector(uint32_t currunt_sector,uint32_t offset);
void flash_enter_power_down(void);
void flash_wake_up(void);





#endif

