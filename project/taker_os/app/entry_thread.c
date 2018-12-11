#include "entry_thread.h"
#include <stdint.h>
#include <string.h>
#include "app_error.h"
#include "boards.h"
#include "app_timer.h"
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
#include "ble_statck_thread.h"
#include  "main_deamon.h"
#include "env.h"
#include "twi_master.h"
#include "spi_bus.h"
#include "nrf_drv_gpiote.h"
#include "tiw_bus.h"
#include "ble_server.h"
#include "ble_nus.h"
st_env    *p_env_local;

extern  void services_init(void);
extern void start_adv(uint8_t  if_paring);
extern uint16_t  m_conn_handle;
extern ble_nus_t m_nus; 
static void general_gpio_init(void)
{
    nrf_gpio_cfg_output(POWER_HOLD);
    nrf_gpio_pin_set(POWER_HOLD);
	nrf_gpio_cfg_output(RED_LED);
	nrf_gpio_cfg_output(GREEN_LED);
	nrf_gpio_cfg_output(BLUE_LED);
	nrf_gpio_pin_set(RED_LED);
	nrf_gpio_pin_set(GREEN_LED);
	nrf_gpio_pin_set(BLUE_LED);
    nrf_gpio_cfg_input(I2C_SCL, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(I2C_SDA, NRF_GPIO_PIN_NOPULL);     
    nrf_gpio_cfg_output(TOUCH_PAD_VDD_EN);
	nrf_gpio_pin_clear(TOUCH_PAD_VDD_EN);
	nrf_gpio_cfg_input(BUTTON_DET, NRF_GPIO_PIN_NOPULL);  //��Դ����
    nrf_gpio_cfg_input(CHARG_STAT, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(HALF_PRODUCT_DTM_PIN, NRF_GPIO_PIN_PULLDOWN);//���Ʒ����       
    nrf_gpio_cfg_input(USB_DET, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_output(IIC_DATA_INT);
    nrf_gpio_pin_clear(IIC_DATA_INT);
    nrf_gpio_cfg_output(SPI_CS);
    nrf_gpio_pin_set(SPI_CS);   
	nrf_gpio_cfg_input(BUTTON_PAGE_DOWN,  NRF_GPIO_PIN_PULLDOWN);  //�Ϸ�ҳ
	nrf_gpio_cfg_input(BUTTON_PAGE_UP,  NRF_GPIO_PIN_PULLDOWN);  //�·�ҳ
	
}
uint8_t check_if_enter_test(void)
{
    if(true == nrf_gpio_pin_read(HALF_PRODUCT_DTM_PIN ))  //���Ʒ����
	{
        uint32_t test_mode_cnt = 0;
            
        while(test_mode_cnt <= 20)
        {
          if(false == nrf_gpio_pin_read(HALF_PRODUCT_DTM_PIN ))
           {
             break;
           }
          test_mode_cnt++;
          vTaskDelay(10);
        }
            if(test_mode_cnt >= 20)					
            {
                    uint8_t res1,res2;                    
                    nrf_gpio_pin_clear(POWER_HOLD);      
                    TOUCH_PAD_ON();    
                    NRF_LOG_INFO("extral device check\r\n");                             
                    res1= flash_hw_check();           //����ⲿfalsh
                    res2=rtc_hw_check();             //���rtc   
                    if(res1||res2)
                    {
                         NRF_LOG_INFO("test error\r\n");   
                    }
                    else
                    {
                         NRF_LOG_INFO("test ok\r\n");   
                    }
                    while(1)
                    {                       
                          vTaskDelay(10);
                    }				            
            }       
	}
    else
    {
        return 1;
        
    }
        
}


void entry_thread(void * arg)
{
//    general_gpio_init();       
//    NRF_LOG_DEBUG("entry start\r\n");
//    nrf_gpio_pin_set(POWER_HOLD);//��ס��Դ
//    app_timer_init(0,0,NULL,NULL); //�����ʱ����ʼ��  
//    //��ʼ����Χ�豸�õ�����������
//    tiw_bus_init();
//    spi_master_init();
//    nrf_drv_gpiote_init();
//    EMR_pad_version();
//    
//    //����Э��ջ�߳�
//    if (pdPASS != xTaskCreate(ble_stack_thread, "BLE", 256, NULL, 3, NULL))
//    {
//        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
//    }
//    
//    env_init();
//    p_env_local=load_env_from_flash();  //��ȡ�豸��Ϣ����������     
//    check_if_enter_test();//����Ƿ���Ҫ�������ģʽ  
//    //��ʼ������
//     NRF_LOG_DEBUG("services_init");  
//     services_init();  //����������ǰ����д    
//   //�������ػ��߳�  ���뻷������ָ��
//    if (pdPASS != xTaskCreate(mian_deamon_thread, "mian deamon", 256,p_env_local,2, NULL))
//    {
//        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
//    }  
//    
//    //�˳�����߳�
//   // exit:   
//     vTaskDelete( NULL );

    //����Э��ջ�߳�
    if (pdPASS != xTaskCreate(ble_stack_thread, "BLE", 256, NULL, 3, NULL))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }

     NRF_LOG_DEBUG("services_init");  
     services_init();  //����������ǰ����д   
    
     start_adv(1);
    
    while(1)
    {
        if(m_conn_handle!=BLE_CONN_HANDLE_INVALID)
        {
            if(m_nus.is_notification_enabled)
            {
            
                uint8_t testbuf[BLE_NUS_MAX_DATA_LEN]={0};
                uint32_t err;
                for(uint8_t cnt=0;cnt<sizeof(testbuf);cnt++)
                {
                    testbuf[cnt]=cnt;
                }
                          
               err= ble_nus_string_send(&m_nus, testbuf,sizeof(testbuf));
                if(err)
                {
                    NRF_LOG_ERROR("error %d\r\n",err);
                }
                else
                {
                     NRF_LOG_DEBUG("ok");
                }
            }
            
        }
        //������� ����100�ֽ�����
        //NRF_LOG_DEBUG("loop \r\n");  
        vTaskDelay(10);
        
    }
    
    
}



