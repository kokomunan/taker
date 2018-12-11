#ifndef __NOTE_MANAGER_H_
#define __NOTE_MANAGER_H_
#include  "stdint.h"

#include "main_deamon.h"

#define UPLOAD_MAX_LEN BLE_NUS_MAX_DATA_LEN

typedef struct  
{
    uint16_t note_head_flag;
    uint32_t note_number;
    uint32_t note_len;
    uint8_t note_time_year;
    uint8_t note_time_month;
    uint8_t note_time_day;
    uint8_t note_time_hour;
    uint8_t note_time_min;
  
} st_note_header_info;
typedef struct  
{
    uint8_t y_h : 5;
    uint8_t store_flag : 3;
    uint8_t y_l;
    uint8_t x_h;
    uint8_t x_l;
    uint8_t press;
} st_store_info;

typedef struct  
{
 uint8_t note_manager_buffer[2][256];         //一个用于存储 另一个用于写入flash
 uint8_t note_manager_write_buffer_index;     //指示哪一个缓冲用于写入flash
 uint32_t note_manager_write_buffer_offset;   //写入flash 偏移
 uint8_t note_manager_read_buffer_index;     //指示哪一个缓冲用于读取 
 uint16_t note_manager_read_buffer_offset;   //用于读取坐标时缓冲的偏移
 uint32_t note_manager_store_cur_sector;    //当前用于存储内容时的扇区
 uint32_t note_manager_note_head_sector;    //笔记头应该存在的扇区
 uint8_t note_manager_valid;                //指示是否还能存储
 uint8_t note_manager_sector_erased;        //扇区是否已经擦除过
 st_note_header_info note_header;           //离线笔记头

}st_note_manager_store_cb;

typedef struct  
{
uint32_t note_manager_upload_start_addr;
uint32_t note_manager_upload_addr_offset;
uint32_t note_manager_upload_len;
uint8_t  note_manager_upload_flag;
st_note_header_info note_header;

}st_note_manager_upload_cb;

typedef struct  
{
uint8_t note_store_leave_flag;
uint8_t note_store_leave_flag_count;
uint8_t note_store_leave_report_flag;
uint8_t note_store_cnt;
uint32_t store_pos_x ;
uint32_t store_pos_y ;

}st_note_manager_position_sovle_cb;

void init_note_manager( st_device *p_device);



void offline_note_storage(uint8_t *buf);
void close_note(uint8_t is_store, uint8_t show);
void upload_stored_note_poll(void);
void offline_write_flash_poll(void);
uint8_t read_note_header(uint16_t start_sector);

#endif


