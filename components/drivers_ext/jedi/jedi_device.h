#ifndef _JEDI_DEVICE_H
#define _JEDI_DEVICE_H


#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "boards.h"

#define ORG_POS_DATA_BUFF_COUNT     3   //坐标缓冲包个数
typedef struct
{
    uint8_t data[8];
}stPosdata;
typedef struct
{
    uint8_t data_count;
    
    stPosdata pos_data[ORG_POS_DATA_BUFF_COUNT];
    
}stPosBuff;

struct BLEDataFormat
{
    uint8_t identifier;
    uint8_t opcode;
    uint8_t length;
    uint8_t payload[20];//00 button init 01 cmd init 02offline running
};
#define TOUCH_PAD_ON()          do{\
                                    nrf_gpio_pin_set(TOUCH_PAD_VDD_EN);\
                                }while(0)
                        								
#define TOUCH_PAD_OFF()          do{\
                                    nrf_gpio_pin_clear(TOUCH_PAD_VDD_EN);\
                                }while(0)	

void Open_jedi(void);
void close_jedi(void);
void EMR_pad_version(void);
void init_position_buffer(void);
void iic_data_interrupt_handler(void);
uint8_t i2c_device_write(uint8_t device_address,uint8_t reg_address, uint8_t *data, uint8_t data_length);
void send_to_slave(uint8_t opcode, uint8_t * data, uint16_t len);
#endif

