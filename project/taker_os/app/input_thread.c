#include "input_thread.h"
#include "env.h"
#include "nrf_log.h"
#include "main_deamon.h"
#include "boards.h"
#include "app_config.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "server_thread.h"
static st_device    * p_device_local_input_deamon;

void power_button_short_release(void)
{

}
void power_button_double_press(void)
{
	uint8_t rst;
    rst = rst;
	if (DEVICE_ONLINE == p_device_local_input_deamon->status)
	{
			uint8_t btn 		= BUTTON_EVENT_CREATE_PAGE;
			
			server_notify_host(BLE_CMD_BTN_EVENT, &btn, 1);

	}

	if (DEVICE_OFFLINE == p_device_local_input_deamon->status)
	{
		//close_note(1, 1);
	}
}
void power_button_long_press(void)
{

    p_device_local_input_deamon->status = DEVICE_TRYING_POWER_OFF;
    NRF_LOG_INFO("trying power off!");
}
void input_deamon(void * arg)
{
    
    p_device_local_input_deamon= (st_device *)arg;
    button_init();          
    while(1)
    {
        ButtonProcessLoop();
    }
  
}
void input_init(void * arg)
{
 
    if (pdPASS != xTaskCreate(input_deamon, "input", 128,arg,2, NULL))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }   
    
}
