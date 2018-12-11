
#include    <stdlib.h>
#include   	"jedi_device.h"
#include 	"boards.h"
#include 	"tiw_bus.h"
#include 	"nrf_delay.h"
#include   "note_manager.h"
#include    "nrf_log.h"
#include    "FreeRTOS.h"
#include    "task.h"
#include "semphr.h"
#include "nrf_drv_gpiote.h"
#include "main_deamon.h"
#include "app_config.h"
#include "get_position.h"

#define  I2C_MTU   32
uint8_t pad_version[] = {0x00, 0x01, 0x02, 0x06, 0x03, 0x02};
uint32_t write_signal_count = 0;
//int32_t x_last_pos;
//int32_t y_last_pos;
//uint32_t last_time;
uint16_t jedi_version=0;
uint8_t nortify_enable=0;
TaskHandle_t   read_i2c_data_handle;
SemaphoreHandle_t m_sem_i2c_read_ready; 

nrf_drv_gpiote_in_config_t i2c_data_int_config=GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
void position_solve_callback(uint8_t *buf, uint8_t len);
void jedi_data_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

uint8_t i2c_device_write(uint8_t device_address,uint8_t reg_address, uint8_t *data, uint8_t data_length)
{
	uint8_t buff[I2C_MTU];
    
    uint8_t cnt = 0;
	
	memcpy(buff,data,data_length);
    for(cnt = 0; cnt < 200; cnt++)
    {
        if(true == twi_master_tx(device_address, buff, I2C_MTU, false))
        {
       
            break;
        }
    }
    if(cnt < 200)
    {
        return 0;
    }
    return 1;

}

static uint8_t i2c_device_read(uint8_t device_address,uint8_t reg_address, uint8_t *data, uint8_t data_length)
{
    uint8_t config = 0;

  
        if (twi_master_rx(device_address, data, data_length)) // Read: current configuration
        {
            // Read succeeded, configuration stored to variable "config"
            config = true;
        }
        else
        {
            // Read failed
            config = 0;
        }
  
    return config;
}
void EMR_pad_version()
{
    uint8_t tmp[8];
    TOUCH_PAD_ON();
    vTaskDelay(10);
    nrf_gpio_cfg_input(IIC_DATA_INT, NRF_GPIO_PIN_PULLUP);
    if(i2c_device_write(0x88, 0, pad_version, 6))
    {
        while(1 == nrf_gpio_pin_read(IIC_DATA_INT))
        {
            vTaskDelay(5);
        }
			
        i2c_device_read(0x88, 0, tmp, 8);       
        NRF_LOG_INFO("emr pad version: [%d.%d]\r\n", tmp[5], tmp[6]);
    }
    else
    {
       NRF_LOG_ERROR("do not detected EMR\r\n");
    }
    TOUCH_PAD_OFF();
}
void read_i2c_data_thread(void * arg)
{
    //初始化中断触发
     st_device *p_device=arg;
     nrf_drv_gpiote_in_init(IIC_DATA_INT ,&i2c_data_int_config ,jedi_data_interrupt_handler);
     position_solve_init();
    
    while(1)
    {
         //等待信号量   
         //i2c 读取相应的长度  分别进行不同的处理
        
        while (pdFALSE == xSemaphoreTake(m_sem_i2c_read_ready, portMAX_DELAY))
        {
           
        }
        
        //复位关机倒计时
            //int16_t x_pos = 0, y_pos = 0;						
		    uint8_t tmp[I2C_MTU], rst = 0;
            rst = i2c_device_read(0x88, 0, tmp, 8);			
            if(rst)
            {
                
                if(tmp[0] == 0x02)   //position
                {

                        // x_pos = (uint16_t)tmp[3] << 8 | tmp[2];
                        // y_pos = (uint16_t)tmp[5] << 8 | tmp[4];                                                                     
                       // DEBUG_LOG("x%d,y%d",x_pos,y_pos);    
                       //  rst = check_position_data(x_pos, y_pos);  //暂时不做坐标飞笔过滤
                       // if(rst)
                      //  {
                    
                           if(p_device->status==DEVICE_OFFLINE )
                           {
                               position_solve_offline_callback(tmp, 8);                                                         
                               
                           }                          
                           else if(p_device->status==DEVICE_ONLINE)
                           {
                               
                               position_solve_online_callback(tmp, 8);
                           }
                           
    
                        //    write_signal_count++;
                      //  }
                     
                }                                       
            }	
    }    
}
void open_jedi(void *arg)
{
    NRF_LOG_INFO("open jedi");
    TOUCH_PAD_ON();  //打开电源    
    vTaskDelay(500);
     m_sem_i2c_read_ready = xSemaphoreCreateBinary();
    if (NULL == m_sem_i2c_read_ready)
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
    if (pdPASS !=  xTaskCreate(read_i2c_data_thread, "i2c read", 256, arg, 4, &read_i2c_data_handle)) //创建一个用于接收处理中断数据的任务
    {
       APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
 
}
void close_jedi(void) //关闭中断响应和删除创建的任务  
{
    if(read_i2c_data_handle!=NULL)
    {
       vTaskDelete(&read_i2c_data_handle);        
       read_i2c_data_handle=NULL; 
    }

    if(m_sem_i2c_read_ready!=NULL)
    {
        vSemaphoreDelete(&m_sem_i2c_read_ready);
    }
    
    nrf_drv_gpiote_in_uninit(IIC_DATA_INT );
      
}


//uint8_t check_position_data(int32_t x_pos, int32_t y_pos)
//{
//    uint32_t time_interval = 0;
//    uint32_t x_pos_interval = 0;
//    uint32_t y_pos_interval = 0;

//    if(system_tick_counts >= last_time)
//    {
//        time_interval = system_tick_counts - last_time;
//    }
//    else
//    {
//        time_interval = 0xffffffff - last_time + system_tick_counts;
//    }
//    last_time = system_tick_counts;
//    
//    
//    
//    if(time_interval > 1)
//    {
//        
//        x_last_pos = x_pos;
//        y_last_pos = y_pos;
//        return true;
//    }
//    else
//    {
//        //online mode data
//        x_pos_interval = abs(x_pos - x_last_pos);
//        y_pos_interval = abs(y_pos - y_last_pos);
//        if(x_pos_interval > ORG_POSITION_MAX_CHANGE_NUM || (y_pos_interval > ORG_POSITION_MAX_CHANGE_NUM))
//        {   
//			DEBUG_LOG("position error");
//            return false;
//        }
//        x_last_pos = x_pos;
//        y_last_pos = y_pos;
//        return true;
//    }
//}
void jedi_data_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{ 
   
    if((pin==IIC_DATA_INT )&&(action==NRF_GPIOTE_POLARITY_HITOLO))
    {
         BaseType_t yield_req = pdFALSE;
         xSemaphoreGiveFromISR(m_sem_i2c_read_ready, &yield_req );
        
    }

}

void send_to_slave(uint8_t opcode, uint8_t * data, uint16_t len)
{
	struct BLEDataFormat  report;
	
	report.identifier=0xaa;
	report.opcode=opcode;
	report.length=len;
	if(len>18)
	{
		len=18;
	}
	memcpy(report.payload,data,len);
	i2c_device_write(0x88,0, (uint8_t *)&report, len+3);
	
}




