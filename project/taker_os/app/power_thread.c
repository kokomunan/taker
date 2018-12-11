#include "power_thread.h"
#include "main_deamon.h"
#include "boards.h"
#include "env.h"
#include "nrf_log.h"
#include "app_timer.h"
#include "battery.h"
#include "FreeRTOS.h"
#include "task.h"
static st_device    *p_device_local;
static uint32_t m_power_off_time_count;
APP_TIMER_DEF(m_bat_check_timer_id);   
      
 void battery_check_timer_callback(void * p_context)
 {
     
    battery_check_one_time();  //经行电池电压检测

 }     
         
 void power_deamon_thread(void * arg)
{
    
   p_device_local=(st_device *) arg;        
   m_power_off_time_count=0;
   batt_meas_init();      
   app_timer_create(&m_bat_check_timer_id,APP_TIMER_MODE_REPEATED,battery_check_timer_callback);//创建定时检测电量定时器  
   app_timer_start(m_bat_check_timer_id, 2000, NULL);
          
  while(1)
  {

    if(nrf_gpio_pin_read(USB_DET))
    {
        
        if((DEVICE_ONLINE ==p_device_local->status)||(DEVICE_OFFLINE == p_device_local->status))
        {
            m_power_off_time_count++;
            if(m_power_off_time_count >= p_device_local->p_env->device_var_info.poweroff_countdown_time)
            {  
                p_device_local->status=  DEVICE_TRYING_POWER_OFF;  
                m_power_off_time_count = 0;
                NRF_LOG_INFO("timeout power off");
            }
        }
	
    }
    
    if(nrf_gpio_pin_read(USB_DET))  //无usb插入
    {
      
			if(batt_meas_is_shutdown())
			{
				              
                p_device_local->status=  DEVICE_TRYING_POWER_OFF;  
                
			}            
			if(battery_not_enough())
			{	
                   //周期性提示电量不足
		
			}
    }
    else
    {   
        if(nrf_gpio_pin_read(CHARG_STAT))  //充满
        {
                 //如果之前不是充满 
                //设置正在充电中
        }
        else
        { 
            //如果之前不是充电中
            //设置成充电中
			
            
        }
			      

     }
    
    vTaskDelay(100);//100ms 经行一次检测

  }      
    
    
      
    
}
