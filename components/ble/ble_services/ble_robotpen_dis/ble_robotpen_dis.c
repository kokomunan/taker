/**
 * Copyright (c) 2012 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/* Attention!
*  To maintain compliance with Nordic Semiconductor ASA’s Bluetooth profile
*  qualification listings, this section of source code must not be modified.
*/
#include "sdk_common.h"

#include "ble_robotpen_dis.h"
#include <stdlib.h>
#include <string.h>
#include "app_error.h"
#include "ble_gatts.h"
#include "ble_srv_common.h"


#define BLE_UUID_ROBOTPEN_DIS_SERVER 0x0000    
#define BLE_UUID_ROBOTPEN_DIS_FW_CHARACTERISTIC 0x0001                     
#define BLE_UUID_ROBOTPEN_DIS_HW_CHARACTERISTIC 0x0002
#define BLE_UUID_ROBOTPEN_DIS_DEVICE_CHARACTERISTIC 0x0003
#define BLE_UUID_ROBOTPEN_DIS_POS_CHARACTERISTIC 0x0004
#define BLE_UUID_ROBOTPEN_DIS_MAC_CHARACTERISTIC 0x0005






#define ROBOTPEN_DIS_BASE_UUID           {{0xfd,0xb8,0x17,0x55,0x09,0xab,0x06,0x49,0xb9,0xf3,0xbc,0x4a,0x00,0x00,0xc2,0x0e}} /**< Used vendor specific UUID. */
/**@brief Function for adding the Characteristic.
 *
 * @param[in]   uuid           UUID of characteristic to be added.
 * @param[in]   p_char_value   Initial value of characteristic to be added.
 * @param[in]   char_len       Length of initial value. This will also be the maximum value.
 * @param[in]   dis_attr_md    Security settings of characteristic to be added.
 * @param[out]  p_handles      Handles of new characteristic.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t char_add(uint16_t                        uuid,
                         uint8_t                       * p_char_value,
                         uint16_t                        char_len, 
						 uint16_t                        service_handle,
                         ble_gatts_char_handles_t*       p_handles )
{
    ble_uuid_t          ble_uuid;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_gatts_attr_md_t attr_md;

    APP_ERROR_CHECK_BOOL(p_char_value != NULL);
    APP_ERROR_CHECK_BOOL(char_len > 0);

    // The ble_gatts_char_md_t structure uses bit fields. So we reset the memory to zero.
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read  = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf        = NULL;
    char_md.p_user_desc_md   = NULL;
    char_md.p_cccd_md        = NULL;
    char_md.p_sccd_md        = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, uuid);

    memset(&attr_md, 0, sizeof(attr_md));

	
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
			
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = char_len;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = char_len;
    attr_char_value.p_value   = p_char_value;

    return sd_ble_gatts_characteristic_add(service_handle, &char_md, &attr_char_value, p_handles);
}


uint32_t ble_robotpen_dis_init(ble_robotpen_dis_s* p_robopen_dis ,const ble_robot_dis_init_t * p_dis_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;
    ble_uuid128_t nus_base_uuid = ROBOTPEN_DIS_BASE_UUID;
    // Add service
	
	
	VERIFY_PARAM_NOT_NULL(p_robopen_dis);
    VERIFY_PARAM_NOT_NULL(p_dis_init);
	
	err_code = sd_ble_uuid_vs_add(&nus_base_uuid, &p_robopen_dis->uuid_type);
    VERIFY_SUCCESS(err_code);
	
	ble_uuid.type = p_robopen_dis->uuid_type;
    ble_uuid.uuid = BLE_UUID_ROBOTPEN_DIS_SERVER;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_robopen_dis->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }


	err_code = char_add(BLE_UUID_ROBOTPEN_DIS_FW_CHARACTERISTIC,
						(uint8_t*)&p_dis_init->fw_version,
						sizeof(uint32_t),
						p_robopen_dis->service_handle,	
						&p_robopen_dis->fw_rev_handles);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	
    err_code = char_add(BLE_UUID_ROBOTPEN_DIS_HW_CHARACTERISTIC,
						(uint8_t*)&p_dis_init->hw_version,
						sizeof(uint16_t),
						p_robopen_dis->service_handle,	
						&p_robopen_dis->hw_rev_handles);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	
	err_code = char_add(BLE_UUID_ROBOTPEN_DIS_DEVICE_CHARACTERISTIC,
						(uint8_t*)&p_dis_init->device_id,
						sizeof(uint8_t),
						p_robopen_dis->service_handle,	
						&p_robopen_dis->device_id_rev_handles);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}
   
	
	err_code = char_add(BLE_UUID_ROBOTPEN_DIS_POS_CHARACTERISTIC,
						(uint8_t*)&p_dis_init->dis_pos,
						sizeof(ble_robot_dis_pos_st),
						p_robopen_dis->service_handle,	
						&p_robopen_dis->dis_pos_handles);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	
	err_code = char_add(BLE_UUID_ROBOTPEN_DIS_MAC_CHARACTERISTIC,
						(uint8_t*)&p_dis_init->mac,
						6,
						p_robopen_dis->service_handle,	
						&p_robopen_dis->mac_handles);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	
		
		
    return NRF_SUCCESS;
}

