#ifndef BLE_SERVER_H_
#define BLE_SERVER_H_
#include "stdint.h"



enum
{
    INITIALIZATION = 0x00,
    BLE_PAIRING          = 0x01,
    BLE_RECONNECTING          = 0x02,
    BLE_CONNECTED  = 0x03,
 
};



void    Open_ble(void);
void    Close_ble(void);
uint8_t Ioctl_ble(uint8_t cmd,uint8_t *para);
uint8_t Read_ble(uint8_t *msg,uint8_t len);
uint8_t Write_ble(uint8_t opcode, uint8_t * data, uint16_t len);
void    manager_ble(void)  ;
uint8_t ble_direct_write (uint8_t *data ,uint8_t len);
void    ble_clear_buff(void);
void set_ble_status(uint16_t connect_handle,uint8_t status);
#endif

