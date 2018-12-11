#include "ble_server.h"
#include <stdint.h>
#include <stdlib.h>
#include "boards.h"
#include "nrf_error.h"
#include "server_thread.h"
#include "ble_advertising.h"
#include "ble_hci.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ble_nus.h"
#include "nrf_queue.h"
#include "nrf_log.h"
#include "ble_client_response.h"
#include "app_config.h"
extern void advertising_init(uint8_t ad_type);
extern ble_nus_t m_nus;
static uint16_t m_connect_handle;
void start_adv(uint8_t  if_paring);
static uint8_t  m_ble_sta=0;



NRF_QUEUE_DEF(uint8_t, ble_send_queen, BLE_NUS_MAX_DATA_LEN*20, NRF_QUEUE_MODE_OVERFLOW)   ;
NRF_QUEUE_INTERFACE_DEC(uint8_t, ble_send_queen);
NRF_QUEUE_INTERFACE_DEF(uint8_t, ble_send_queen, &ble_send_queen);


NRF_QUEUE_DEF(uint8_t, ble_recive_queen, BLE_NUS_MAX_DATA_LEN*10, NRF_QUEUE_MODE_OVERFLOW)   ;
NRF_QUEUE_INTERFACE_DEC(uint8_t, ble_recive_queen);
NRF_QUEUE_INTERFACE_DEF(uint8_t, ble_recive_queen, &ble_recive_queen);


SERVIVE_OPS  ble_ops={
	
    0,
    0,
    Open_ble,
    Close_ble,
    Ioctl_ble,
    Read_ble,
    Write_ble,
    manager_ble,
    ble_clear_buff,
    ble_direct_write ,	
};

void Open_ble(void)
{
    ble_clear_buff();
}

void  Close_ble(void)
{
    
    if( ble_ops.status==BLE_CONNECTED )
    {     
         uint32_t err_code=sd_ble_gap_disconnect(m_connect_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
         APP_ERROR_CHECK(err_code);       
         vTaskDelay(50);        
         err_code = sd_ble_gap_adv_stop();
		 APP_ERROR_CHECK(err_code);
        
    }
    else
    {
        uint32_t err_code = sd_ble_gap_adv_stop();
		APP_ERROR_CHECK(err_code);
        
    }
  	
}

//操作蓝牙设备

uint8_t Ioctl_ble(uint8_t cmd,uint8_t *para)
{
   switch(cmd)
   {
       case START_WORK:  
           
            if(para)
            {
                start_adv(1);
            
            }
            else
            {
                start_adv(0);
            }
                 
           break;
       
       case INIT:
           
                ble_client_init((st_device *)para);
             
           break;
       
       
   }
    
   return 0;

}
uint8_t Read_ble(uint8_t *msg,uint8_t len)  //提供给nus 接受
{
	
    
     if((0xAA == *msg) && (*(msg+1) >= BLE_CMD_STATUS))
    {
        uint8_t buff[BLE_NUS_MAX_DATA_LEN];
        memset(buff,0,BLE_NUS_MAX_DATA_LEN);
        memcpy(buff,msg,len);
        return nrf_queue_write(&ble_recive_queen, msg, BLE_NUS_MAX_DATA_LEN);
    }
    else
    {
        return 1;
    }

}

//返回非零代表错误  将要写入的数据放入缓冲池中
uint8_t Write_ble(uint8_t opcode, uint8_t * data, uint16_t len)
{
   
   
    uint32_t error;
    if(ble_ops.status != BLE_CONNECTED)
    {
        return 1;  

    }
    if(len >BLE_NUS_MAX_DATA_LEN )
    {
        
        NRF_LOG_ERROR("err due to len ");
        return 1; 
    }

    error=nrf_queue_write(&ble_send_queen, data, BLE_NUS_MAX_DATA_LEN);
    if(error)
    {
       return 1;  
    }
     return 0;  
     
	
}

void manager_ble(void)  	//蓝牙事件处理
{

    //如果状态变化回调设备状态处理
    if(ble_ops.status == BLE_CONNECTED)
    {
        
        uint32_t err;
        uint8_t buff[BLE_NUS_MAX_DATA_LEN];
        
        while(nrf_queue_read(&ble_send_queen,buff,BLE_NUS_MAX_DATA_LEN)==0)
        {
            
           err=  ble_nus_string_send(&m_nus, buff, BLE_NUS_MAX_DATA_LEN);  
            
           if(err)
           {
               break;

           }                                   
        }
                       
         while(nrf_queue_read(&ble_recive_queen,buff,BLE_NUS_MAX_DATA_LEN)==0)
        {
            
            ble_client_response(buff, BLE_NUS_MAX_DATA_LEN);
        }
                         
    }
      
    if( m_ble_sta!=ble_ops.status)
    {
        ble_ops.status= m_ble_sta;
        server_status_call_back(ble_ops.status);  
    }
    
    
   
}
uint8_t ble_direct_write (uint8_t *data ,uint8_t len)
{      
    return   ble_nus_string_send(&m_nus, data, len);
       
}

void ble_clear_buff(void)
{
    
    nrf_queue_reset(&ble_send_queen);
    nrf_queue_reset(&ble_recive_queen);
	
}

void set_ble_status(uint16_t connect_handle,uint8_t status)
{
    
    m_ble_sta=status;
    m_connect_handle=connect_handle;
}


void start_adv(uint8_t  if_paring)
{
    uint32_t err_code;
    advertising_init(if_paring);
    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);//开始广播
    APP_ERROR_CHECK(err_code);      
    
}
