#ifndef _SERVER_THREAD_H
#define _SERVER_THREAD_H



#include "stdint.h"
#include "main_deamon.h"

typedef struct  
{

	uint8_t ready;
	uint8_t status;
    void    (* Open)      (void);
	void    (*Close)     (void);
    uint8_t (* Ioctl)(uint8_t cmd,uint8_t *);
    uint8_t (* Read)(uint8_t *, uint8_t );   
	uint8_t (*Write)     (uint8_t opcode, uint8_t * data, uint16_t len) ;
	void    (*manager)   (void);	
	void    (*clear_buff)  (void);
	uint8_t (*Write_direct)  (uint8_t *data ,uint8_t len);
	
} SERVIVE_OPS;

enum IOCTL_CMD{
	
	INIT=0x00,  //²éÑ¯¾ÍÐ÷
	START_WORK,
	STOP_WORK,
    SET_PARA,
    
	
};

enum SERVER_MODE{
	
    BLE_MODE,   
    UNSUPORT_MODE,
	
	
};


void server_thread(void* arg);
uint8_t server_init(uint8_t mode,st_device *p_device);
void server_stop(void);
uint8_t get_server_mode(void);
uint8_t server_Send_position_to_host(uint8_t opcode, uint8_t * data, uint16_t len);
uint8_t  server_notify_host(uint8_t opcode, uint8_t * data, uint16_t len);
uint8_t server_ioctl(uint8_t cmd,uint8_t *data);
void server_status_call_back(uint8_t sta);
uint8_t server_send_direct(uint8_t *data ,uint8_t len);
uint8_t server_recive(uint8_t * data, uint8_t len);
uint8_t get_server_status(void);
uint8_t get_server_ready(void);
void set_server_ready(uint8_t v);
void server_start(uint8_t mode,st_device *p_device);
void usb_server_close(void);
void usb_server_open(void);

#endif

