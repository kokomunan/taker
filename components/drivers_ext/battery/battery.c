/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision$
 */
 
#include "battery.h"
#include "boards.h"
#include "nrf_assert.h"
#include "nrf_error.h"
#include "nrf_gpio.h"
#include "nrf_drv_saadc.h"
#include "nrf_delay.h"
#include "LED.h"
#include "nrf_log.h"
#include "FreeRTOS.h"
#include "task.h"
#define BATT_OFFSET 22

//extern stMainStatus g_main_status;
uint8_t last_battery_value = 7;
uint8_t charge_status =  NOCHARGE;
uint16_t battery_percent = 0;

typedef enum
{
       BATTERY_100P,
       BATTERY_80P,
       BATTERY_60P,
       BATTERY_40P,
       BATTERY_20P,
       BATTERY_05P,
       BATTERY_00P
}BatteryTableOffset;

const uint16_t BatteryVoltageTable[7] = 
{
   4250,4066,3909,3809,3737,3650,3605   //97,80,64,48,32,16,0
};
uint8_t get_battery_percent(uint16_t BatteryVoltageInMV)
{

       uint8_t  tmpBatteryPercent;

       if(BatteryVoltageInMV>= BatteryVoltageTable[BATTERY_100P])
       {
            tmpBatteryPercent = 7;
       }
       else if(BatteryVoltageInMV> BatteryVoltageTable[BATTERY_80P])
       {      
            tmpBatteryPercent = 6;
       }
       else if(BatteryVoltageInMV> BatteryVoltageTable[BATTERY_60P])
       {      
            tmpBatteryPercent = 5;
       }
       else if(BatteryVoltageInMV> BatteryVoltageTable[BATTERY_40P])
       {
            tmpBatteryPercent = 4;
       }
       else if(BatteryVoltageInMV> BatteryVoltageTable[BATTERY_20P])
       {
            tmpBatteryPercent = 3;
       }
       else if(BatteryVoltageInMV> BatteryVoltageTable[BATTERY_05P])
       {
            tmpBatteryPercent = 2;
       }
       else if(BatteryVoltageInMV> BatteryVoltageTable[BATTERY_00P])
       {
            tmpBatteryPercent = 1;
       }
       else
       {
            tmpBatteryPercent = 0;
       }
       return tmpBatteryPercent;
}

uint16_t batt_meas_update(void)
{ 
    uint32_t error;
    uint32_t m_batt_adc_val = 0;
    uint32_t m_batt_mv_val = 0;        
    error =nrf_drv_saadc_sample_convert(NRF_SAADC_INPUT_AIN7, (int16_t *)& m_batt_adc_val);
    if(error)
    {      
      return 0;  
    }
    
    m_batt_mv_val = ADC_TO_VBAT_MV(m_batt_adc_val);
    m_batt_mv_val -= BATT_OFFSET;//电量偏移补偿
    return get_battery_percent(m_batt_mv_val);    
   
}
void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{

}

void saadc_init(void)
{
    ret_code_t err_code;    
    last_battery_value = 7;
    charge_status =  NOCHARGE;
    battery_percent = 0;   
    nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN7);
    err_code = nrf_drv_saadc_init(NULL, saadc_callback);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);

}


uint32_t batt_meas_init(void)
{
    
    uint32_t err_code;     
    saadc_init();  
    
    battery_percent =batt_meas_update();	 //第一次检测
    if(nrf_gpio_pin_read(USB_DET))//no USB
    {   
          charge_status= NOCHARGE;
    }
    else
    {
        if(true == nrf_gpio_pin_read(CHARG_STAT))  //加入电池量判定
        {
            charge_status= CHARGE_FULL; //charging full
        }
        else
        {
            charge_status =   CHARGING; //charging
        }
    }
    
    
    return err_code;
}

bool battery_not_enough(void)
{
    if(battery_percent <= LOW_BAT_THRESHOLD)
    {
        return true;
    }
    return false;
}
bool Is_battery_is_safe(void)
{
    if(battery_percent>= OTA_BAT_THRESHOLD)
    {
        return true;
    }
    return false;
       
}

bool batt_meas_is_shutdown(void)
{
    if(battery_percent <= SHUTDOWN_THRESHOLD)
    {
        return true;
    }
    return false;
}
bool batt_meas_power_check(void)
{
    if(battery_percent<= POWE_ON_THRESHOLD)
    {
        return true;
    }
    return false;
}
uint8_t power_on_battery_check(void)
{

    if(batt_meas_power_check())
    {
        NRF_LOG_INFO("battery low, can not power on!");
              
        for (uint8_t i = 0; i <= 3; i++)
        {   
            RED_LED_ON();
            vTaskDelay(200);
            RED_LED_OFF();
            vTaskDelay(200);
        }
    
        return false ;
    }
    else
    {
        return true ; 
    }

}
void battery_check_one_time(void)
{   
    battery_percent=  batt_meas_update();    
    if(nrf_gpio_pin_read(USB_DET))
    {
        if(battery_percent < last_battery_value)
        {
          
            battery_percent=last_battery_value;
         
        }
        else
        {
            
            last_battery_value = battery_percent;                      
        }
    }
    else
    {
        last_battery_value = battery_percent;    
    } 
        
}
