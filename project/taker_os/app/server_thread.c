#include "server_thread.h"
#include "env.h"
#include "nrf_log.h"
#include "main_deamon.h"
#include "boards.h"
#include "app_config.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ble_server.h"
#include "LED.h"
extern SERVIVE_OPS ble_ops;
static uint8_t 		Server_mode = 0;
static SERVIVE_OPS *	server_ops;
static st_device *p_device_local;
static TaskHandle_t  server_thread_handle;
uint8_t server_init(uint8_t mode,st_device *p_device)
{
	uint8_t 		res;
	uint16_t		delay_ms = 0;

	Server_mode 		= mode;
     p_device_local=p_device;
	if (mode == BLE_MODE)
	{
		server_ops			= (SERVIVE_OPS *) &ble_ops;
		server_ops->Open();
		delay_ms			= 0;
		while (nrf_gpio_pin_read(BUTTON_DET)==BUTTONS_ACTIVE_STATE)
		{
			vTaskDelay(5);
			delay_ms++;
			if (delay_ms == 400)
			{
				               
                BLUE_LED_ON();//��������                               
			}                  
			else if (delay_ms > 1000)
			{
				break;
			}
		}
		if (delay_ms >=400)
		{
            uint8_t 		param;
            
            server_ops->Ioctl(INIT,(uint8_t *)p_device);
            
            if(delay_ms>1000)
            {
                  param = 1  ;           
            }
            else
            {
                  param = 0;
            }
            res = server_ops->Ioctl(START_WORK, &param);
            if (res)
            {
                NRF_LOG_ERROR("ble has error");
                NVIC_SystemReset();
            }
			
		}
		else 
		{
			return 1;
		}
        
        if (pdPASS != xTaskCreate(server_thread, "BLE server", 256, NULL, 2, &server_thread_handle))
        {
            APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
        }
	//	init_offline_store_data();
	//	TOUCH_PAD_ON();
        //������������߳�
        
		return 0;
	}
	else //�Զ�����
	{

		uint8_t 		server_mode;

		server_mode 		= BLE_MODE; 			
		NRF_LOG_INFO("server init at mode%d", server_mode);       
		server_start(server_mode,p_device);
//		Display_init();
//		input_init();
//		Set_Device_Status(DEVICE_OFFLINE);              
//		DEBUG_LOG("start menu");
        //������������߳�
        if (pdPASS != xTaskCreate(server_thread, "BLE server", 256, NULL, 2, NULL))
        {
            APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
        }
		return 0;
	}

}

void server_thread(void* arg)
{
    
    
    
    while(1)
    {
        server_ops->manager();
        vTaskDelay(10);
        
    }
      
}
//�����п���һ���µķ���
void server_start(uint8_t mode,st_device *p_device)
{
	uint8_t 		res;
	Server_mode 		= mode;
	if (mode == BLE_MODE)
	{
		server_ops = (SERVIVE_OPS *) &ble_ops;
		server_ops->Open();
        server_ops->Ioctl(INIT,(uint8_t *)p_device);
        NRF_LOG_INFO("prepare to connecting\r\n");
        {
            uint8_t 		param = 0;  
            res = server_ops->Ioctl(START_WORK, &param);
            if (res)
            {
                NRF_LOG_ERROR("ble has error");
                NVIC_SystemReset();
            }
        }			
	}
}
//ֹͣһ������
void server_stop(void)
{
	if (server_ops != NULL)
	{
		server_ops->Close();
	}
	server_ops			= NULL;
	//TOUCH_PAD_OFF();
    //flash_wake_up();  
	//close_note(1, 0);   
	//page_det_stop();
	//Set_Device_Status(DEVICE_OFFLINE);				//�����豸״̬Ϊ����
}
uint8_t server_notify_host(uint8_t opcode, uint8_t * data, uint16_t len)
{
	uint8_t 		res;
	if (server_ops != NULL)
	{
		res 				= server_ops->Write(opcode, data, len); //��Ҫ���͵����ݷ��绺���
	}
	return res;
}
//����û�н��뻺��� ����ֱ�ӷ��͸�ble ���ͳ�ȥ
uint8_t server_send_direct(uint8_t * data, uint8_t len)
{
	uint8_t 		res;
	if (server_ops != NULL)
	{
		res 				= server_ops->Write_direct(data, len);
	}
	return res;
}

uint8_t server_recive(uint8_t * data, uint8_t len)
{
    uint8_t 		res;
	if (server_ops != NULL)
	{
		res 				= server_ops->Read(data,len);
	}
	return res;
    
}
//�����豸״̬�Ļص��������˺����ɲ�ͬ�������豸���ã������������Լ���״̬
void server_status_call_back(uint8_t sta)
{
	if (Server_mode == BLE_MODE)
	{

		if (BLE_CONNECTED == sta)
		{
			NRF_LOG_INFO("ble connected host");
			//TOUCH_PAD_ON();
			 p_device_local->status = DEVICE_ONLINE;
			server_ops->clear_buff();
            //LED  UI
            
            
		}
		else if (BLE_PAIRING == sta)
		{
			
          

		}
		else 
		{
			NRF_LOG_INFO("ble disconnected host");
			if ((DEVICE_POWER_OFF != p_device_local->status) &&
				 (DEVICE_TRYING_POWER_OFF != p_device_local->status))
			{
				
				p_device_local->status = DEVICE_OFFLINE;
                //����LED ui
				//set_en_upload(0);
				//TOUCH_PAD_ON();
				//flash_wake_up();
			}
		}
	}

}
//��ȡ�����豸��״̬
uint8_t get_server_status(void)
{
	if (server_ops != NULL)
	{
		return server_ops->status;
	}
	else 
	{
		return INITIALIZATION;
	}
}


uint8_t server_ioctl(uint8_t cmd, uint8_t * data)
{
	return server_ops->Ioctl(cmd, data);
}


//��ȡ��ǰ������豸
uint8_t get_server_mode(void)
{
	return Server_mode;
}


uint8_t get_server_ready(void)
{
	return server_ops->ready;
}


void set_server_ready(uint8_t v)
{
	server_ops->ready	= v;
}


void update_battery(uint8_t bat_level)
{
    //�����ǰ������ģʽ  
    //����������ظ��·���
    //    uint32_t err_code;
//    uint8_t  battery_level;

//    err_code = ble_bas_battery_level_update(&m_bas, battery_level);
//    if ((err_code != NRF_SUCCESS) &&
//        (err_code != NRF_ERROR_INVALID_STATE) &&
//        (err_code != BLE_ERROR_NO_TX_PACKETS) &&
//        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
//       )
//    {
//        APP_ERROR_HANDLER(err_code);
//    }
    
    
}







