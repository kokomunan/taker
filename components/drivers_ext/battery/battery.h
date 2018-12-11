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

/** @file Battery module.
 *
 * @defgroup modules_batt_meas
 * @{
 * @ingroup nrfready_modules
 * @brief 
 *
 * @details This module deals with battery measurements.
 *          The figure below depicts how this module operates:
 * @image html flow_m_batt_meas.png Battery measurement module flow 
 */
#ifndef __M_BATT_MEAS_H__
#define __M_BATT_MEAS_H__

#include <stdint.h>
#include <stdbool.h>

#define SHUTDOWN_THRESHOLD      0
#define POWE_ON_THRESHOLD       1
#define LOW_BAT_THRESHOLD       2
#define OTA_BAT_THRESHOLD       4

enum {
    
  NOCHARGE,
  CHARGING , 
  CHARGE_FULL,   
    
    
    
};


#define ADC_VDIV_R1_10K         20	//20K ohms
#define ADC_VDIV_R2_10K         62	//59K ohms

/*** ADC VREF = 1.2V internal bandgap voltage ***/
// input no pre-scale
#define ADC_REF_VBG_10V         12	//1.2V


#define ADC_TO_VBAT_MV(adc)      ((adc)*(ADC_VDIV_R1_10K + ADC_VDIV_R2_10K)*ADC_REF_VBG_10V*1000)/(10*ADC_VDIV_R1_10K*1024)
/**@brief Battery measurement initialization
 * 
 * @param[in] event_callback Event callback called when change in battery level exceeds threshold
 * @param[in] threshold      Required change in voltage to generate event [%]
 * @return 
 * @retval NRF_SUCCESS
 * @retval NRF_ERROR_INVALID_PARAM
 */
uint32_t batt_meas_init(void);
uint16_t batt_meas_update(void); 
bool batt_meas_is_lowbatt(void);
bool batt_meas_is_shutdown(void);
bool batt_meas_power_check(void);
bool battery_not_enough(void);
uint16_t batt_meas_get_value(void);
uint8_t get_battery_percent(uint16_t BatteryVoltageInMV);
uint16_t batt_meas_get_mv_value(void);

 uint8_t power_on_battery_check(void);
uint16_t batt_meas_get_value(void);
void battery_check_one_time(void);
void low_power_mode_poll(void);


#endif /* __M_BATT_MEAS_H__ */
