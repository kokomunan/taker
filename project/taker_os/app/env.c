#include "env.h"
#include "fstorage.h"
#include "app_error.h"
#include "stdio.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "nrf_log.h"
st_env     m_enviroment;
static SemaphoreHandle_t   m_sema_wait_erase_finish;
static SemaphoreHandle_t   m_sema_wait_store_finish;

void env_var_callback(fs_evt_t const * const evt, fs_ret_t result)
{
    
     BaseType_t yield_req = pdFALSE;
    if(evt->id==FS_EVT_STORE)
    {   
    
         xSemaphoreGiveFromISR(  m_sema_wait_store_finish, &yield_req );
    }
    else
    {
         xSemaphoreGiveFromISR( m_sema_wait_erase_finish, &yield_req);
    }
     
}


FS_REGISTER_CFG(fs_config_t   device_var_info_config) =
{
    .callback  = env_var_callback,
    .num_pages = 1,
    .p_start_addr=(uint32_t *)DEVICE_VAR_INFO_START_ADR,
    .p_end_addr=(uint32_t *)DEVICE_VAR_INFO_END_ADR,
    .priority  = 0xFe
};


void env_init(void)
{
    uint32_t err_code;
    err_code =  fs_init();
    APP_ERROR_CHECK(err_code);
    m_sema_wait_erase_finish = xSemaphoreCreateBinary();//创建信号量
    if (NULL == m_sema_wait_erase_finish)
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
    m_sema_wait_store_finish = xSemaphoreCreateBinary();//创建信号量
    if (NULL == m_sema_wait_store_finish)
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
    
}


uint8_t update_var_env_to_flash(void)
{
    uint32_t err_code;
    
    err_code= fs_erase(&device_var_info_config, (uint32_t *)DEVICE_VAR_INFO_START_ADR,1,NULL);
    APP_ERROR_CHECK(err_code); 

     while (pdFALSE == xSemaphoreTake(m_sema_wait_erase_finish, portMAX_DELAY))
    {
      
    }
                                        
     err_code= fs_store(&device_var_info_config, (uint32_t *)DEVICE_VAR_INFO_START_ADR,(uint32_t *)&m_enviroment.device_var_info,sizeof(st_device_var_info)/4,NULL);              
     APP_ERROR_CHECK(err_code); 
     while (pdFALSE == xSemaphoreTake(m_sema_wait_store_finish, portMAX_DELAY))
    {
      
    }
    return 0;
   
}

st_env  * load_env_from_flash(void)
{
    
   
    memcpy((uint8_t *)&m_enviroment.device_constant_info,(uint8_t *)DEVICE_CONSTANT_INFO_START_ADR,sizeof(st_device_constant_info));
    memcpy((uint8_t *)&m_enviroment.device_var_info,(uint8_t *)DEVICE_VAR_INFO_START_ADR,sizeof(st_device_var_info)); 

    if(m_enviroment.device_var_info.identifier!= 0xdead)
    {
        NRF_LOG_INFO("set default info\r\n");
        memset((uint8_t *)&m_enviroment.device_var_info, 0, sizeof(st_device_var_info));
        m_enviroment.device_var_info.identifier = 0xdead;
        m_enviroment.device_var_info.last_connected_flag = 0;
        m_enviroment.device_var_info.firmware_version = SW_VERSION;
        m_enviroment.device_var_info.stored_total_note_num = 0;
        m_enviroment.device_var_info.note_read_start_sector = 0;
        m_enviroment.device_var_info.note_read_end_sector = 0;       
        m_enviroment.device_var_info.error_code = 0;
        m_enviroment.device_var_info.note_index=0;      
		m_enviroment.device_var_info.poweroff_countdown_time=DEFAULT_POWEROFF_COUNTDOWN_TIME;							         
        update_var_env_to_flash();
      
        
    }
    
    NRF_LOG_INFO("stored notes[%d]\r\n", m_enviroment.device_var_info.stored_total_note_num);
    NRF_LOG_INFO("product id[0x%x]sw version[0x%x]'\r\n",m_enviroment.device_constant_info.product_id ,m_enviroment.device_var_info.firmware_version);

    return &m_enviroment;


}
