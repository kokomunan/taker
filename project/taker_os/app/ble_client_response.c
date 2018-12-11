#include "ble_client_response.h"
#include "app_config.h"
#include "nrf_log.h"
#include "app_config.h"
#include "main_deamon.h"
#include "string.h"
#include "server_thread.h"
#include "boards.h"
static st_device *p_device_local;
void send_error_msg(uint8_t error)
{
    server_notify_host(BLE_CMD_ERROR_MSG, &error, 1);
}

void ble_client_init(st_device *p_device)
{
    p_device_local=p_device;
    
}

void ble_client_response(void * p_event_data, uint16_t event_size)
{
    uint8_t op_code = *((uint8_t *)p_event_data + 1);
    uint8_t len = *(uint8_t *)((uint8_t *)p_event_data + 2);
    uint8_t *payload = ((uint8_t *)p_event_data + 3);
		
    switch(op_code)
    {
        case BLE_CMD_STATUS:
        {		
			
          NRF_LOG_INFO(" get status ");
        }break;
        case BLE_CMD_NAME_SET:
        {
            uint8_t name_f = 0;
            NRF_LOG_INFO("name set[%d]", len);              
            NRF_LOG_INFO("%s",(uint32_t )payload);

            if(len > 0)
            {
              
                name_f = *payload;
                if((name_f < 48) || (name_f > 122))
                {
                    send_error_msg(ERROR_NAME_CONTENT);
                    break;
                }
                               
                memcpy(p_device_local->p_env->device_var_info.name.device_name, payload, len);               
                p_device_local->p_env->device_var_info.name.device_name_flag = 1;
                p_device_local->p_env->device_var_info.name.device_name_length = len;
                server_notify_host(BLE_CMD_NAME_SET, NULL, 0);
            }
        }break;
        case BLE_CMD_GET_VERSION:
        {
            
            NRF_LOG_INFO("require device version");
            st_version_info version;
            version.hw_version = p_device_local->p_env->device_constant_info.hard_version;
            version.ble_fw_version = SW_VERSION;
			p_device_local->p_env->device_var_info.IOS_link_code=payload[0];          
			NRF_LOG_INFO("get link code[%x]", payload[0]);			
            server_notify_host(BLE_CMD_GET_VERSION, (uint8_t *)&version, sizeof(st_version_info));
	
        }break;
        case BLE_CMD_REQUIRE_MAC:
        {
            NRF_LOG_INFO("Get Mac");

            server_notify_host(BLE_CMD_REQUIRE_MAC, (uint8_t *)&p_device_local->p_env->device_constant_info.ble_addr.addr[0], 6);

            NRF_LOG_INFO("Send Mac addr[%x][%x][%x][%x][%x][%x]", p_device_local->p_env->device_constant_info.ble_addr.addr[0],
											  p_device_local->p_env->device_constant_info.ble_addr.addr[1],
											  p_device_local->p_env->device_constant_info.ble_addr.addr[2],
                                              p_device_local->p_env->device_constant_info.ble_addr.addr[3],
											  p_device_local->p_env->device_constant_info.ble_addr.addr[4],
											  p_device_local->p_env->device_constant_info.ble_addr.addr[5]);
			
        }
		break;
		case BLE_CMD_REQUIRE_NAME:
		{
			NRF_LOG_INFO("reqiure name");
			uint8_t device_name[16];
			uint8_t len;
			
			if(p_device_local->p_env->device_var_info.name.device_name_flag ==0)
			{
				memset(device_name,0,16);
				
				sprintf((char *)device_name, "%s",DEVICE_DEFAULT_NAME);
				len=strlen((const char*)device_name);
				
			}
			else
			{              
				len=p_device_local->p_env->device_var_info.name.device_name_length;
				memcpy(device_name,p_device_local->p_env->device_var_info.name.device_name,len );	
				
			}			              		 
            server_notify_host(BLE_CMD_REQUIRE_NAME, device_name, len );

		}					
		break;
        case BLE_CMD_RTC_SET:
        {
            
            st_RTC_info rtc_data;
			uint8_t res=0;
            memcpy((uint8_t *)&rtc_data, payload, sizeof(st_RTC_info));
            NRF_LOG_INFO("rtc set[%d][%d][%d][%d][%d]", rtc_data.note_time_year,
                                                       rtc_data.note_time_month,
                                                       rtc_data.note_time_day,
                                                       rtc_data.note_time_hour,
                                                       rtc_data.note_time_min);
               
            write_rtc_data(&rtc_data);
			server_notify_host(BLE_CMD_RTC_SET, (uint8_t *)&res, 1);  //增加了对设置时间的操作回应
			
        }break;
        
//    case BLE_CMD_ENTER_OTA_MODE:
//     {
//				uint8_t fw_num;
//        uint16_t ble_version,mcu_version;
//			 
//        NRF_LOG_INFO("enter ota mode");
//			 
//        if((DEVICE_ONLINE == g_main_status.device_state) && ( (USB_STATUS == nrf_gpio_pin_read(USB_DET)) || battery_value >= 4))
//         {
//						g_main_status.device_state = DEVICE_OTA_MODE;

//					  led_dis_enable = 1;
//						LEDStart(UI_OTA);
//					 
//						if(get_cur_server_mode()==USB_MODE)//如果是USB模式 
//						{
//							mcu_version=payload[0]<<8|payload[1];
//							ble_version=payload[2]<<8|payload[3];					
//						}else{
//							mcu_version=payload[2]<<8|payload[3];
//							ble_version=payload[0]<<8|payload[1];
//						}
//		
//						NRF_LOG_INFO("version %x",mcu_version<<8|ble_version);
//						ble_need_update=0;
//						mcu_need_update=0;
//						ota_step=START_DFU;		
//						if(mcu_version>jedi_version)
//						{
//							mcu_need_update=1;
//						}
//						if(ble_version>SW_VERSION)
//						{
//							ble_need_update=1;				
//						}
//				
//						if(mcu_need_update)
//						{
//							//如果是USB模式 步骤为下载中 让从机进入USB模式的升级
//							ota_step=GET_MCU_FW_INFO;	
//							NRF_LOG_INFO("mcu start update");
//							send_to_slave(CMD_ENTER_DFU, payload, len);//发送给slave
//						}else if(ble_need_update)
//						{
//							ota_step=GET_BLE_FW_INFO;	
//							init_ota_status();			
//							NRF_LOG_INFO("ble start update");		
//							send_to_slave(CMD_ENTER_DFU, NULL, 0);//设置从机状态
//							vTaskDelay(20);
//					
//							fw_num=BLE_FW_NUM;					
//							server_notify_host(BLE_CMD_OTA_FILE_INFO, &fw_num, 1);
//						}else{
//							NRF_LOG_INFO("version error");	
//							send_error_msg(ERROR_OTA_VERSION);//无固件可升级
//						}													
//					}else{
//            send_error_msg(ERROR_OTA_STATUS);
//          }
//        }
//		break;
//    case BLE_CMD_OTA_FILE_INFO:
//    {
//			uint32_t fw_len;
//			
//      NRF_LOG_INFO("ota file info");
//			if(DEVICE_OTA_MODE == g_main_status.device_state)
//      {
//				memcpy((uint8_t *)&fw_len,payload,4);
//				if(ota_step==GET_MCU_FW_INFO)  //mcu
//				{
//					NRF_LOG_INFO("mcu lenth[%d]", fw_len);				
//					ota_step=DOWNLOAD_MCU_FW;//设置状态为 下载中
//					send_to_slave(CMD_GET_FW_INFO, (uint8_t *)&fw_len, 4);//发送给slave
//				}else if(ota_step==GET_BLE_FW_INFO){
//					ota_step=DOWNLOAD_BLE_FW;				
//					//st_ota_info ota_info;
//					g_ota_record.flow_num = 0;
//					NRF_LOG_INFO("ble lenth[%d]", fw_len);	
//				
//					if(fw_len < PSTORAGE_MAX_APP_SIZE)
//					{
//						g_ota_record.ota_file_total_len = fw_len;						
//						ota_settings.firmware_length = fw_len;
//						
//						m_ota_storage_handle.block_id = PSTORAGE_OTA_SWAP_ADDR;
//						if(pstorage_clear( &m_ota_storage_handle , 1024))
//						{
//							NRF_LOG_INFO("erase flash fialed");
//							
//						}
//						m_ota_storage_handle.block_id += 1024;
//					}else{
//						NRF_LOG_INFO("len error[%d]",fw_len);
//						send_error_msg(ERROR_OTA_STATUS);
//					}	
//				}else{
//					send_error_msg(ERROR_OTA_STATUS);
//				}				
//      }else{
//				NRF_LOG_INFO("get info has error");
//        send_error_msg(ERROR_OTA_STATUS);
//      }
//    }
//		break;
//    case BLE_CMD_OTA_RAW_DATA:
//        {

//            if(DEVICE_OTA_MODE == g_main_status.device_state)
//            {
//				if(get_cur_server_mode()==USB_MODE)
//				{
//					vTaskDelay(10);
//				}
//												
//				if(ota_step==DOWNLOAD_MCU_FW)
//				{
//					NRF_LOG_INFO("send fw data to slave");
//					send_to_slave(CMD_GET_RAW_DATA , payload, len);//发送给slave
//					
//					
//					
//				}
//				else if(ota_step==DOWNLOAD_BLE_FW)
//				{
//					uint8_t flow_num = payload[0];
//					uint8_t* raw_data = &payload[1];
//					uint8_t cnt = 0;
//					uint8_t raw_len = 0;
//					raw_len = len - 1;
//					if(flow_num != g_ota_record.flow_num)
//					{
//						NRF_LOG_INFO("flow number error[%d][%d]",flow_num, g_ota_record.flow_num);
//						send_error_msg(ERROR_OTA_FLOW_NUM);
//						server_notify_host(BLE_CMD_OTA_RAW_DATA, &g_ota_record.flow_num, 1);
//						break;
//					}
//					for(cnt = 0; cnt < raw_len; cnt++)
//					{
//						g_ota_record.ota_checksum += raw_data[cnt];
//					}
//					if(raw_len < MAX_OTA_PAYLOAD_LEN)
//					{
//					
//						if((g_ota_record.ota_store_buff_len + raw_len) < OTA_RCV_MAX_BUFF_LEN)
//						{
//							memcpy((uint8_t *)&ota_rec_buffer[g_ota_record.ota_store_buff_len], raw_data, raw_len);
//							g_ota_record.ota_store_buff_len += raw_len;
//							g_ota_record.ota_downloaded_len += raw_len;
//							//copy the last packet to flash
//							pstorage_store( &m_ota_storage_handle,  (uint8_t *)&ota_rec_buffer[0],  g_ota_record.ota_store_buff_len,  0);
//						}
//						else
//						{
//							if((g_ota_record.ota_store_buff_len + raw_len) == OTA_RCV_MAX_BUFF_LEN)
//							{
//								memcpy((uint8_t *)&ota_rec_buffer[g_ota_record.ota_store_buff_len], raw_data, raw_len);
//								g_ota_record.ota_downloaded_len += raw_len;
//								g_ota_record.ota_store_buff_len += raw_len;
//								pstorage_store( &m_ota_storage_handle,  (uint8_t *)&ota_rec_buffer[0],  0x400,  0);
//								g_ota_record.ota_store_buff_len = 0;
//							}
//			   
//						}
//					}
//					else
//					{
//						g_ota_record.flow_num++;
//						if((g_ota_record.ota_store_buff_len + MAX_OTA_PAYLOAD_LEN) < OTA_RCV_MAX_BUFF_LEN)
//						{
//							memcpy((uint8_t *)&ota_rec_buffer[g_ota_record.ota_store_buff_len], raw_data, MAX_OTA_PAYLOAD_LEN);
//							//NRF_LOG_INFO("buffer len[%d]", g_ota_record.ota_store_buff_len);
//							g_ota_record.ota_store_buff_len += MAX_OTA_PAYLOAD_LEN;
//							g_ota_record.ota_downloaded_len += MAX_OTA_PAYLOAD_LEN;
//							server_notify_host(BLE_CMD_OTA_RAW_DATA, &(g_ota_record.flow_num), 1);
//						}
//						else
//						{
//							if((g_ota_record.ota_store_buff_len + MAX_OTA_PAYLOAD_LEN) == OTA_RCV_MAX_BUFF_LEN)
//							{
//								memcpy((uint8_t *)&ota_rec_buffer[g_ota_record.ota_store_buff_len], raw_data, MAX_OTA_PAYLOAD_LEN);
//								pstorage_store( &m_ota_storage_handle, (uint8_t *)&ota_rec_buffer[0],  0x400,  0);
//								m_ota_storage_handle.block_id += 1024;
//								g_ota_record.ota_store_buff_len = 0;
//								g_ota_record.ota_downloaded_len += MAX_OTA_PAYLOAD_LEN;
//							}
//			   
//						}
//					}
//					
//					
//				}
//				else
//				{
//					
//					send_error_msg(ERROR_OTA_STATUS);
//				}
//											
// 
//            }
//            else
//            {
//				
//                send_error_msg(ERROR_OTA_STATUS);
//            }
//        }break;
//        case BLE_CMD_OTA_CHECKSUM:
//        {
//          if(DEVICE_OTA_MODE == g_main_status.device_state)
//						{
//							if(ota_step==GET_MCU_FW_CHECK)
//								{
//									NRF_LOG_INFO("send fw checksum to slave");
//									send_to_slave( CMD_GET_CHEKSUM , payload, len);//发送给slave
//								}else if(ota_step==GET_BLE_FW_CHECK)
//								{										
//									uint32_t host_checksum = 0;
//								//uint32_t check = 0;
//									uint8_t result = 1;
//									
//								//host_checksum = (uint32_t)payload;
//									memcpy((uint8_t *)&host_checksum, payload, 4);
//									NRF_LOG_INFO("checksum[%d]-[%d]", host_checksum, g_ota_record.ota_checksum);
//									if(host_checksum != g_ota_record.ota_checksum)
//										{
//											uint8_t fw_num;
//										//checksum error
//											send_error_msg(ERROR_OTA_CHECKSUM);
//											server_notify_host(BLE_CMD_OTA_RESULT, &result, 1);
//											ota_step=GET_BLE_FW_INFO;	
//											init_ota_status();				
//											fw_num=BLE_FW_NUM;					
//											server_notify_host(BLE_CMD_OTA_FILE_INFO, &fw_num, 0);  //校验失败 重新下载请求
//										}else{
//											result = 0;
//											server_notify_host(BLE_CMD_OTA_RESULT, &result, 1);
//											ota_step=FINISH_DFU;	
//											if(get_cur_server_mode()==USB_MODE)//如果是USB模式 设置标志  如果蓝牙也升级了 通知设置标记 发送切换指令  复位
//												{
//													
//												
//													uint8_t send_byte[3];
//													send_byte[0] = 0xAA;
//													send_byte[1] = BLE_CMD_OTA_SWTICH;
//													send_byte[2] = 0;

//													send_direct_host((uint8_t *)&send_byte, 3);  //这个地方如果发送 失败尝试重新发送
//													if(mcu_need_update)   
//														{
//															NRF_LOG_INFO("set auto boot to slave");
//															send_to_slave(CMD_NOTIFY_RESET , payload, len);//发送给slave
//															vTaskDelay(500);	
//														}
//													//set dirty flag

//													ota_settings.ota_state = OTA_SWITCH_NORDIC;			//~~~~~			 
//													if( pstorage_otasetting_info())
//													{
//														NRF_LOG_INFO("store ota setting info  fialed");
//														
//													}//~~~~~~~~~~~~~~~~														
//													//set auto-poweron flag
//													g_device_info.auto_poweron = 1;
//												
//													if(pstorage_device_info())
//													{
//														NRF_LOG_INFO("store device info fialed");
//													}

//													NRF_LOG_INFO("reset!");
//													NVIC_SystemReset();
//													while(0);				  				  
//												}
//										}
//								}else{
//									send_error_msg(ERROR_OTA_STATUS);
//								}
//							}else{
//                send_error_msg(ERROR_OTA_STATUS);
//							}
//        }
//				break;
//        case BLE_CMD_OTA_SWTICH:
//        {
//            NRF_LOG_INFO("ota switch");
//            if(g_main_status.device_state ==DEVICE_OTA_MODE)
//            {
//				
//				
//				  if(GET_MCU_FW_CHECK== ota_step)
//				  {
//					
//					  
//					    if(ble_need_update)   //如果蓝牙需要升级
//						{
//							uint8_t fw_num;
//							ota_step=GET_BLE_FW_INFO;	
//							init_ota_status();				
//							fw_num=BLE_FW_NUM;					
//							server_notify_host(BLE_CMD_OTA_FILE_INFO, &fw_num, 1);
//							NRF_LOG_INFO("continue update ble ");
//							
//							
//						}
//						else    //设置自动重启
//						{

//							uint8_t send_byte[3];
//							send_byte[0] = 0xAA;
//							send_byte[1] = BLE_CMD_OTA_SWTICH;
//							send_byte[2] = 0;
//							send_direct_host((uint8_t *)&send_byte, 3);  //这个地方如果发送 失败尝试重新发送


//							NRF_LOG_INFO("set auto boot to slave");
//							send_to_slave(CMD_NOTIFY_RESET , payload, len);//发送给slave
//							vTaskDelay(500);
//							//set auto-poweron flag
//							g_device_info.auto_poweron = 1;
//							
//							if(pstorage_device_info())
//							{
//								NRF_LOG_INFO("store device info fialed");
//								
//							}else{
//								NRF_LOG_INFO("store device info success 1");
//							}
//							NRF_LOG_INFO("reset!");
//							NVIC_SystemReset();
//							while(1);
//							
//													
//							
//						}
//					  					  	
//					  
//				  }
//				  else if(FINISH_DFU== ota_step) //升级完成
//				  {
//					  
//					  
//				
//						uint8_t send_byte[3];
//						send_byte[0] = 0xAA;
//						send_byte[1] = BLE_CMD_OTA_SWTICH;
//						send_byte[2] = 0;

//						send_direct_host((uint8_t *)&send_byte, 3);  //这个地方如果发送 失败尝试重新发送
//					  
//					  
//					  if(mcu_need_update)   //如果蓝牙需要升级
//				     {
//						 NRF_LOG_INFO("set auto boot to slave");
//						 send_to_slave(CMD_NOTIFY_RESET , payload, len);//发送给slave
//						 vTaskDelay(500);
//							
//					 }
//					  					  
//						//set dirty flag

//						ota_settings.ota_state = OTA_SWITCH_NORDIC;						 
//						if( pstorage_otasetting_info())
//						{
//							NRF_LOG_INFO("store ota setting info  fialed");
//							
//						}
//													 
//						//set auto-poweron flag
//						g_device_info.auto_poweron = 1;
//						
//						if(pstorage_device_info())
//						{
//							NRF_LOG_INFO("store device info fialed");
//							
//						}else{
//							NRF_LOG_INFO("store device info success 2");
//						}
//						NRF_LOG_INFO("reset!");
//						NVIC_SystemReset();
//						while(1);				  				  
//					  
//				  }
//				     
//              
//            }
//            else
//            {
//                send_error_msg(ERROR_OTA_STATUS);
//            }
//        }break;
//        case BLE_CMD_OTA_QUIT:
//        {
//            if(DEVICE_OTA_MODE == g_main_status.device_state)
//            {
//                NRF_LOG_INFO("Quit OTA!");
//                g_main_status.device_state = DEVICE_ONLINE;
//								send_to_slave(CMD_QUIT_DFU, NULL, 0);
//							  led_dis_enable = 1;
//								LEDStart(UI_CONNECTED);
//            }
//            else
//            {
//                send_error_msg(ERROR_OTA_STATUS);
//            }
//        }break;
//        
//        case BLE_CMD_SYNC_MODE_ENTER:
//        {
//					RED_LED_OFF();
//            NRF_LOG_INFO("enter sync mode");
//            if((DEVICE_ONLINE == g_main_status.device_state) 
//                || (DEVICE_LOW_POWER_ACTIVE == g_main_status.device_state))
//            {
//                g_main_status.device_state = DEVICE_SYNC_MODE;
////				TOUCH_PAD_OFF() ;//关闭iic 电源
//				memset(&offline_note_header,0,sizeof(st_note_header_info));
//                flash_wake_up();
//				server_notify_host( CMD_ENTER_SYC, NULL,0);//通知从机进入同步模式
//                
//            }
//            else
//            {
//                send_error_msg(ERROR_OTA_STATUS);
//            }
//        }break;
//        case BLE_CMD_SYNC_MODE_QUIT:
//        {
//            if(DEVICE_SYNC_MODE == g_main_status.device_state)
//            {
//                NRF_LOG_INFO("Quit sync!");
//				note_upload_flag = 0;
//				init_offline_store_data();
//                g_main_status.device_state = DEVICE_ONLINE;
////				TOUCH_PAD_ON();
//             
//				server_notify_host(  CMD_QUI_SYC, NULL,0);//通知从机退出同步模式
//            }
//            else
//            {
//                send_error_msg(ERROR_OTA_STATUS);
//            }
//        }break;
//        case BLE_CMD_SYNC_FIRST_HEADER:  //开始搜索一个笔记的第一个笔记头
//        {
//            if(DEVICE_SYNC_MODE == g_main_status.device_state)
//            {     
//                //report note header
//                if(g_device_info.stored_total_note_num)
//                {
//					NRF_LOG_INFO("search from[%d] sector to[%d] sector", g_device_info.note_read_start_sector, g_device_info.note_read_end_sector);
//                    if(read_note_header(g_device_info.note_read_start_sector))
//                    {
//                        note_upload_start_addr = offline_note_header.note_start_sector * FLASH_SECTOR_SIZE + 256;
//						//NRF_LOG_INFO("note data start addr[%x]",note_upload_start_addr);
//                        note_store_addr_offset = 0;
//                        note_upload_flag = 0;
//						note_upload_len=0;
//                        server_notify_host(BLE_CMD_SYNC_FIRST_HEADER, (uint8_t *)&offline_note_header, sizeof(st_note_header_info));//这个地方
//                        //NRF_LOG_INFO("note num[%d][%d]", note_header.note_number, note_header.note_len);
//                    }
//					else
//					{
//						 
//						 send_error_msg(ERROR_NO_NOTE);
//	
//					}
//					
//                }
//                else
//                {
//                    send_error_msg(ERROR_NO_NOTE);
//                }
//            }
//            else
//            {
//                send_error_msg(ERROR_OTA_STATUS);
//            }
//        }break;
//        case BLE_CMD_SYNC_ONCE_START:
//        {
//            if(DEVICE_SYNC_MODE == g_main_status.device_state)
//            {
//                if(note_upload_start_addr)
//                {
//                    //start to report raw data
//                    NRF_LOG_INFO("sync start!");
//                    note_upload_flag = 1;
//                    server_notify_host(BLE_CMD_SYNC_ONCE_CONTENT, NULL, 0);
//					NRF_LOG_INFO("notify the host!");
//                }
//            }
//            else
//            {
//                send_error_msg(ERROR_OTA_STATUS);
//            }
//        }break;
//        case  BLE_CMD_SYNC_IS_END :   //如果相同笔记ID的块返回这个笔记下一个块的头部 如果搜索不到了 就将之前这个笔记ID所有的头部清除掉 然后返回2说明这个笔记传输完可以下一个笔记了
//        {
//									
//             if((DEVICE_SYNC_MODE == g_main_status.device_state) && (0 == note_upload_flag))
//            {
//								
//					uint32_t sec_offset = (offline_note_header.note_len+256) / FLASH_SECTOR_SIZE;	
//					uint32_t if_has_more=0;	
//                    uint8_t end_content = 0x02;
//                    if(((offline_note_header.note_len+256) % FLASH_SECTOR_SIZE)!=0)
//					{
//						if_has_more=1;
//					
//					}
//					else
//					{
//						if_has_more=0;
//					}
//				
//					
//				    NRF_LOG_INFO("more%d offset %d",if_has_more,sec_offset);
//					g_device_info.note_read_start_sector =get_next_sector(g_device_info.note_read_start_sector,if_has_more+ sec_offset);	//得到下一个扇区号	
//					if(g_device_info.stored_total_note_num>0)
//					{
//						g_device_info.stored_total_note_num -= 1;
//					}
//                    note_store_addr_offset = 0;
//					note_upload_len=0;
//                    note_upload_start_addr = 0;
//					g_main_status.flash_store_data_flag=1;
//                    memset((uint8_t *)&offline_note_header, 0, sizeof(st_note_header_info));  //清除找到的笔记 这个地方
//                    NRF_LOG_INFO("total note[%d]", g_device_info.stored_total_note_num);
//					NRF_LOG_INFO("start sector [%d]",g_device_info.note_read_start_sector);							     
//                    pstorage_update(&g_device_info_handle, (uint8_t *)&g_device_info, sizeof(st_device_data_storage), 0);
//                    vTaskDelay(100);                
//                    server_notify_host(BLE_CMD_SYNC_IS_END, &end_content, 1);
//         
//        
//            }
//            else
//            {
//                send_error_msg(ERROR_OTA_STATUS);
//            }
//        }break;

		
    }
}
