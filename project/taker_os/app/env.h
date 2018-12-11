#ifndef _ENV_H_
#define _ENV_H_

#include "stdint.h"
#include "ble_gap.h"

#include "app_config.h"


typedef struct  
{
    uint8_t device_name_flag;
    uint8_t device_name_length;
    uint8_t device_name[18];
} st_device_name;


typedef struct
{
    uint16_t identifier;
    uint8_t no_write_data_send_flag; //1 send no write data, 0 only send write data
    uint8_t last_connected_flag      ;//test flag,used for record if the device has be connected 
    st_device_name name; //20 bytes
    uint8_t auto_poweron;
    uint32_t firmware_version;
    uint32_t stored_total_note_num;  //未同步的笔记数量
    uint32_t note_read_start_sector;
    uint32_t note_read_end_sector;
	uint32_t note_index;   //最新创建的笔记id
    uint32_t error_code;
	uint16_t IOS_link_code;  //连接特征码
	uint16_t align  ;   //这个是为了4字节对齐
    uint32_t poweroff_countdown_time;
} st_device_var_info;


typedef struct
{
	ble_gap_addr_t ble_addr;//7 bytes
	uint8_t  hard_version;
	uint32_t product_id;
}st_device_constant_info;



typedef struct
{
    st_device_var_info   device_var_info;
    st_device_constant_info    device_constant_info;
    
    
}st_env;



void env_init(void);

uint8_t update_var_env_to_flash(void);
st_env  * load_env_from_flash(void);


#endif

