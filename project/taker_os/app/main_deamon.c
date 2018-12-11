#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_bas.h"
#include "ble_dis.h"
#include "ble_conn_params.h"
#include "boards.h"
#include "sensorsim.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "peer_manager.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "fds.h"
#include "fstorage.h"
#include "ble_conn_state.h"
#include "nrf_drv_clock.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "main_deamon.h"
#include "app_config.h"
#include "env.h"
#include "power_thread.h"
#include "server_thread.h"
#include "LED.h"
#include "input_thread.h"

static uint8_t m_poweron_btn_hold_flag=0;
static  st_device  device_info;
static uint8_t m_device_status_last=0;
st_device*  get_device_info(void)
{
 return &device_info;
}

 void state_machine_power_off(void)
{
	if (BUTTONS_ACTIVE_STATE == nrf_gpio_pin_read(BUTTON_DET)) //button detect enabled, and the button is pressed
	{
		if (0 == m_poweron_btn_hold_flag)
		{        
			device_info.status = DEVICE_STANDBY;
			NRF_LOG_DEBUG("enter standby!/r/n");
			return;
		}
	}
	else 
	{
		if (m_poweron_btn_hold_flag)
		{
			m_poweron_btn_hold_flag = 0;
			NVIC_SystemReset();
		}
	}
}

void state_machine_standby(void)
{
    if(BUTTONS_ACTIVE_STATE== nrf_gpio_pin_read(BUTTON_DET))//button detect enabled, and the button is pressed
    {
       
        nrf_gpio_pin_set(POWER_HOLD);
        device_info.status = DEVICE_INIT_BTN;
        //LEDStart(UI_POWER_ON);
        NRF_LOG_DEBUG("enter btn init!");
        return;//this return is must for switch the state machine
    }
}
 uint8_t button_initialization_process(void)
{
 
    if(false == power_on_battery_check())
    {
        return false;
    }
    
    if(server_init(BLE_MODE,&device_info))
    {
        return false; 
    }  

    return true;
}
 void state_machine_button_initialization(void)
{
    if(false == button_initialization_process())
    {
        device_info.status = DEVICE_STANDBY;
        NVIC_SystemReset();
    }
    else
    {
        led_init()	;
        input_init(&device_info);    
        device_info.status = DEVICE_OFFLINE; 
        NRF_LOG_INFO("enter offline!");	
        //初始化离线管理
        
        
    }
}
void state_machine_trying_poweroff(void)
{

 
    server_stop();
   //close_note(1, 0);
    close_jedi();
    led_stop();    
    //保存env到flash 中
    nrf_gpio_pin_clear(POWER_HOLD);
    if(BUTTONS_ACTIVE_STATE == nrf_gpio_pin_read(BUTTON_DET))
    {
        m_poweron_btn_hold_flag = 1;
    }
    else
    {
        NVIC_SystemReset();
    }

}

void state_machine_offline(void)
{
  if(m_device_status_last!=device_info.status)
    {
    
        //唤醒flash
        //打开jedi
        //打开离线管理
        //刷新关机倒计时
        //如果上传标志打开 关闭掉
        m_device_status_last=DEVICE_OFFLINE;
        
    }
    

}

void state_machine_active(void)
{
 if(m_device_status_last!=device_info.status)
    {
    
        //离线管理退出 保存未保存的笔记 
        //上报离线笔记总数给host
        //关闭flash
        //flash 睡眠
        //刷新关机倒计时
        m_device_status_last=DEVICE_ONLINE;
            
        
    }
   
}
void state_machine_ota(void)
{

   
}

void state_machine_sync(void)
{
   
}



void state_machine_loop(void)
{

    if(DEVICE_POWER_OFF ==device_info.status)
    {
        state_machine_power_off();
    }
    else if(DEVICE_STANDBY == device_info.status)
    {
        state_machine_standby();
    }
    else if(DEVICE_INIT_BTN == device_info.status)
    {
        state_machine_button_initialization();
    } 
    else if(DEVICE_OFFLINE == device_info.status)
    {
      
       state_machine_offline();
       
    }
    else if(DEVICE_ONLINE == device_info.status)
    {
                          
        state_machine_active();
    }
    else if(DEVICE_TRYING_POWER_OFF ==device_info.status)
    {
        state_machine_trying_poweroff();
    }
    else if(DEVICE_OTA_MODE == device_info.status)
    {
        state_machine_ota();
    }
    else if(DEVICE_SYNC_MODE== device_info.status)
    {
        
        state_machine_sync();
    }    
 
}
void mian_deamon_thread(void * arg)
{
      memset((void *)&device_info,0,sizeof(st_device));     
      device_info.p_env= (st_env *)arg;
      NRF_LOG_DEBUG("mian deamon start");
      device_info.status= DEVICE_POWER_OFF;//初始化设备状态
      
    //创建电池功耗守护进程
    if (pdPASS != xTaskCreate(power_deamon_thread, "power",128, &device_info, 2, NULL))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
    
    //检查是否ota后自启动
    //如果是ota 自启动  清除标志
      
      nrf_gpio_pin_clear(POWER_HOLD);//去除主电源hold
    
    while(1)
    {
        
        
        
        state_machine_loop();    //处理状态的切换处理与状态跟踪
        vTaskDelay(10); 
        
    }
    
    
    
}
