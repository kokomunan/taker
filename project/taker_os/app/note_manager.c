
#include "note_manager.h"
#include "boards.h"
#include <string.h>
#include "ext_flash.h"
#include  "spi_bus.h"
#include "ex_rtc.h"
#include "nrf_error.h"
#include "env.h"
#include "server_thread.h"
#include "LED.h"
#include "app_config.h"
#include "ble_nus.h"
#include "nrf_log.h"
static  st_device *p_loacal_device;

    
st_note_manager_store_cb            store_cb;
st_note_manager_upload_cb           upload_cb;
st_note_manager_position_sovle_cb   pos_solve_cb;


 uint32_t get_free_sector(void)
{
	if(p_loacal_device->p_env->device_var_info.note_read_start_sector>=p_loacal_device->p_env->device_var_info.note_read_start_sector)
	{
		return (FLASH_MAX_SECTOR_NUM-1)-(p_loacal_device->p_env->device_var_info.note_read_end_sector - p_loacal_device->p_env->device_var_info.note_read_start_sector);
	}
	else
	{
		return  p_loacal_device->p_env->device_var_info.note_read_start_sector-p_loacal_device->p_env->device_var_info.note_read_end_sector-1;
	}
	
}

uint32_t get_next_sector(uint32_t currunt_sector,uint32_t offset)
{

	currunt_sector+=offset;
	if(currunt_sector>=FLASH_MAX_SECTOR_NUM)
	{
		currunt_sector-=FLASH_MAX_SECTOR_NUM;
		
	}
	
	return currunt_sector;
		
}

void init_note_manager( st_device *p_device)
{
    
    p_loacal_device=p_device;
    memset((void *)&store_cb,0,sizeof(st_note_manager_store_cb));
    memset((void *)&upload_cb,0,sizeof(st_note_manager_upload_cb));
    memset((void *)&store_cb,0,sizeof(st_note_manager_position_sovle_cb));   
    store_cb.note_manager_write_buffer_index=0xff;
    
	if(get_free_sector()>0)
	{			
		store_cb.note_manager_valid = 1;
	}
	
}

void start_offline_note_manager(void)
{
    memset((void *)&store_cb,0,sizeof(st_note_manager_store_cb));
    memset((void *)&upload_cb,0,sizeof(st_note_manager_upload_cb));
    memset((void *)&store_cb,0,sizeof(st_note_manager_position_sovle_cb));   
    store_cb.note_manager_write_buffer_index=0xff;
    
    
}


void end_offline_note_manager(void)
{
    
    
    
}

void write_note_header(uint16_t header_sector, uint8_t *pdata, uint16_t len)
{
    uint8_t send_buffer[4];
    uint32_t WriteAddr = header_sector * FLASH_SECTOR_SIZE;
    while(flash_status_check() & 0x01)
    {
        nrf_delay_ms(1);
    }
    flash_write_enable();
    
    send_buffer[0] = FLASH_WRITE_PAGE;
    send_buffer[1] = (uint8_t)((WriteAddr&0x00ff0000)>>16);
    send_buffer[2] = (uint8_t)((WriteAddr&0x0000ff00)>>8);
    send_buffer[3] = (uint8_t)WriteAddr;
    nrf_gpio_pin_clear(SPI_CS);
    spi_send_recv(send_buffer, NULL, 4);
    spi_send_recv(pdata, NULL, len);
    nrf_gpio_pin_set(SPI_CS);
    
}
//write one page about 3.2ms in  real test
void write_note(uint8_t *pdata, uint16_t len)
{	
    uint8_t send_buffer[4];
    uint32_t WriteAddr = store_cb.note_manager_store_cur_sector * FLASH_SECTOR_SIZE + store_cb.note_manager_write_buffer_offset;
    //if the addr is a new sector
    if(0 == store_cb.note_manager_write_buffer_offset)
    {
        //erase the sector
        flash_erase_sector(store_cb.note_manager_store_cur_sector);
        while(flash_status_check() & 0x01)
        {
            nrf_delay_ms(10);
        }
    }
    flash_write_enable();
    send_buffer[0] = FLASH_WRITE_PAGE;
    send_buffer[1] = (uint8_t)((WriteAddr&0x00ff0000)>>16);
    send_buffer[2] = (uint8_t)((WriteAddr&0x0000ff00)>>8);
    send_buffer[3] = (uint8_t)WriteAddr;
    nrf_gpio_pin_clear(SPI_CS);
    spi_send_recv(send_buffer, NULL, 4);
    spi_send_recv(pdata, NULL, len);
    nrf_gpio_pin_set(SPI_CS);  
//	if(len!=256)
//	{
//		note_store_addr_offset += len;      //如果扇区内的偏移大于4096-256  此时len 为256 造成扇区多加了一个  
//	}
//	else
//	{
//		note_store_addr_offset +=256;
//	}
	store_cb.note_manager_write_buffer_offset += len; 
	store_cb.note_header.note_len += len;  //累计笔记的长度   
    if(store_cb.note_manager_write_buffer_offset >= FLASH_SECTOR_SIZE)
    {
		if( get_free_sector()>0)
		{		
			store_cb.note_manager_store_cur_sector=get_next_sector(store_cb.note_manager_store_cur_sector,1);  //导致下个笔记创建起始的位置错误
			p_loacal_device->p_env->device_var_info.note_read_end_sector=get_next_sector(p_loacal_device->p_env->device_var_info.note_read_end_sector,1);

			//note_store_start_sector++;
			//g_device_info.note_read_end_sector++;
			NRF_LOG_DEBUG("currunt note cotent at[%d] sector",store_cb.note_manager_store_cur_sector);
			store_cb.note_manager_write_buffer_offset = 0;
		}
		else
		{
			store_cb.note_manager_valid = 0;
			NRF_LOG_DEBUG("no space to stroe note");
		}
    }
//	NVIC_ClearPendingIRQ(GPIOTE_IRQn);
	//NVIC_EnableIRQ(GPIOTE_IRQn);	
}
void close_note(uint8_t is_store, uint8_t show)
{
    if(0x1985 == store_cb.note_header.note_head_flag)
    {		       
        if(is_store)
        {
			if (show)
			{
                //插入LED显示

			}					
									
			if( p_loacal_device->p_env->device_var_info.stored_total_note_num<MAX_NOTE_ID) 
			{
			
				if(store_cb.note_manager_read_buffer_offset)
				{
					if(get_free_sector()>0)  //检查是否有空间写入
					{			 
						write_note((uint8_t *)&store_cb.note_manager_buffer[store_cb.note_manager_read_buffer_index][0], store_cb.note_manager_read_buffer_offset);  //根据实际长度写不是固定256
						store_cb.note_manager_read_buffer_offset = 0;
					}
				}
				read_rtc_data((st_RTC_info *)&store_cb.note_header.note_time_year);
				if(store_cb.note_header.note_time_year < 17)
				{
					store_cb.note_header.note_time_year = 17;
					store_cb.note_header.note_time_month = 1;
					store_cb.note_header.note_time_day = 1;
					store_cb.note_header.note_time_hour = 1;
					store_cb.note_header.note_time_min = 1;
				}
				write_note_header(store_cb.note_manager_note_head_sector, (uint8_t *)&store_cb.note_header, sizeof(st_note_header_info));           
				store_cb.note_manager_store_cur_sector=get_next_sector(store_cb.note_manager_store_cur_sector,1);  
				p_loacal_device->p_env->device_var_info.stored_total_note_num++;
			
				NRF_LOG_DEBUG("store note num[%d]from[%d]to[%d]sector len[%d] total note[%d]", store_cb.note_header.note_number, 
                store_cb.note_manager_note_head_sector,p_loacal_device->p_env->device_var_info.note_read_end_sector,
                store_cb.note_header.note_len,p_loacal_device->p_env->device_var_info.stored_total_note_num);
	
			}
			else
			{				
				NRF_LOG_DEBUG("can't create notes");
			}
        }
        else
        {
            store_cb.note_manager_store_cur_sector = store_cb.note_manager_note_head_sector;
            p_loacal_device->p_env->device_var_info.note_read_end_sector = store_cb.note_manager_store_cur_sector;
            NRF_LOG_DEBUG("do not store note num[%d]from[%d]to[%d] sector", p_loacal_device->p_env->device_var_info.stored_total_note_num, store_cb.note_manager_note_head_sector,
            p_loacal_device->p_env->device_var_info.note_read_end_sector);
        }
        memset((uint8_t *)&store_cb.note_header, 0 , sizeof(st_note_header_info));

    }
}
//read one page about 3.4ms in  real test
void read_note(uint32_t addr, uint8_t* pdata, uint16_t len)
{
    uint8_t send_buffer[4];
	if(len==0)
	{
		return;
	}
    send_buffer[0] = FLASH_READ_DATA;
    send_buffer[1] = (uint8_t)((addr&0x00ff0000)>>16);
    send_buffer[2] = (uint8_t)((addr&0x0000ff00)>>8);
    send_buffer[3] = (uint8_t)addr;
    while(flash_status_check() & 0x01)
    {
        nrf_delay_ms(1);
    }
    nrf_gpio_pin_clear(SPI_CS);
    spi_send_recv(send_buffer, NULL, 4);
    spi_send_recv(send_buffer, pdata, len);
    nrf_gpio_pin_set(SPI_CS);
}
uint8_t read_note_header(uint16_t start_sector)
{
    uint32_t ReadAddr = start_sector;
    while(ReadAddr != p_loacal_device->p_env->device_var_info.note_read_end_sector)
    {
		NRF_LOG_DEBUG("read sector[%d]", ReadAddr);
        read_note(ReadAddr*FLASH_SECTOR_SIZE, (uint8_t *)&upload_cb.note_header, sizeof(st_note_header_info));
        if(upload_cb.note_header.note_head_flag == 0x1985)
        {
            NRF_LOG_DEBUG("find a note number:[%d]len:[%d]", upload_cb.note_header.note_number, upload_cb.note_header.note_len);
            NRF_LOG_DEBUG("y[%d]m[%d]d[%d]h[%d]m[%d]", upload_cb.note_header.note_time_year, 
                                                    upload_cb.note_header.note_time_month,
                                                   upload_cb.note_header.note_time_day,
                                                   upload_cb.note_header.note_time_hour,
                                                   upload_cb.note_header.note_time_min);
            break;
        }
        ReadAddr =get_next_sector(ReadAddr,1);
    }
    if(ReadAddr !=p_loacal_device->p_env->device_var_info.note_read_end_sector )
    {
        return 1;
    }
    return 0;
}
void upload_stored_note_poll(void)
{
    uint8_t cnt = 0;
    //uint8_t testcnt = 0;
    uint8_t end_content = 0x01;
    uint8_t tmp_buffer[20];
	uint32_t left_len;
	uint32_t  read_start_addr;
	uint32_t read_end_addr; 
	uint32_t  first_read_len;
    uint32_t flash_end_addr=FLASH_MAX_SECTOR_NUM*FLASH_SECTOR_SIZE;	 //最后的地址

    
    if((upload_cb.note_manager_upload_flag) && (DEVICE_SYNC_MODE == p_loacal_device->status))
    {

		for(cnt = 0; cnt < 4; cnt++)
		{
			read_start_addr=upload_cb.note_manager_upload_start_addr + upload_cb.note_manager_upload_addr_offset;  //当前读的起始地址
			if((upload_cb.note_manager_upload_len + UPLOAD_MAX_LEN) <upload_cb.note_header.note_len)  //如果剩下的内容足以填满整个包
			{

				read_end_addr=read_start_addr+UPLOAD_MAX_LEN;  //读完后的最终地址

				if(read_end_addr>flash_end_addr)//如果要跨界读取
				{     
					NRF_LOG_DEBUG("over the end");
					first_read_len=flash_end_addr- read_start_addr;
					read_note(read_start_addr, (uint8_t *)tmp_buffer, first_read_len);  //先读到最尾端					
					left_len=read_end_addr-flash_end_addr;  //剩余读取的长度											
					read_note(0, (uint8_t *)(tmp_buffer+first_read_len), left_len);//从开头读取剩下数据																

					if(NRF_SUCCESS == server_send_direct((uint8_t *)tmp_buffer, UPLOAD_MAX_LEN))  //如果发送成功
					{   
						upload_cb.note_manager_upload_start_addr=0;     //起始地址设置为头
						upload_cb.note_manager_upload_addr_offset = left_len; //偏移设置为剩余的长度
						upload_cb.note_manager_upload_len+=20; //已读的长度累计
					}
					else
					{
						NRF_LOG_DEBUG("send data fialed");
					}



				}
				else//没有跨界时
				{
					read_note(read_start_addr, (uint8_t *)tmp_buffer, UPLOAD_MAX_LEN);	
					NRF_LOG_DEBUG("read len %d form addr %x",UPLOAD_MAX_LEN,read_start_addr);					
					if(NRF_SUCCESS == server_send_direct((uint8_t *)tmp_buffer, UPLOAD_MAX_LEN))
					{
						upload_cb.note_manager_upload_addr_offset += 20;  //偏移累计
						upload_cb.note_manager_upload_len+=20;   //累计长度
					}
					else
					{
						NRF_LOG_DEBUG("send data fialed");
					}

				}
			}
			else
			{        
				// 不足一个包
				if((upload_cb.note_header.note_len - upload_cb.note_manager_upload_len)==0)
				{
					NRF_LOG_DEBUG("lenth is 0");
					upload_cb.note_manager_upload_flag = 0;
					server_notify_host(BLE_CMD_SYNC_IS_END, &end_content, 1);											
					break;
				}											 
				read_end_addr=read_start_addr+(upload_cb.note_header.note_len - upload_cb.note_manager_upload_len);  //读完后的最终地址
		
				if(read_end_addr>flash_end_addr)//如果要写入的范围超过了存储器的头
				{
					NRF_LOG_DEBUG("over the end");
					first_read_len=flash_end_addr- read_start_addr;
					read_note(read_start_addr, (uint8_t *)tmp_buffer, first_read_len);  //先读到最尾端		
					left_len=read_end_addr-flash_end_addr;  //剩余读取的长度		
					read_note(0, (uint8_t *)(tmp_buffer+first_read_len), left_len);//从开头读取剩下数据
					// NRF_LOG_DEBUG("data ;%x\r\n",*tmp_buffer);
					if(NRF_SUCCESS == server_send_direct((uint8_t *)tmp_buffer,upload_cb.note_header.note_len - upload_cb.note_manager_upload_len))
					{   
						upload_cb.note_manager_upload_start_addr=0;     //起始地址设置为头
						upload_cb.note_manager_upload_addr_offset = left_len; //偏移设置为剩余的长度
						upload_cb.note_manager_upload_len+=(upload_cb.note_header.note_len - upload_cb.note_manager_upload_len); //已读的长度累计
						upload_cb.note_manager_upload_flag = 0;
						server_notify_host(BLE_CMD_SYNC_IS_END, &end_content, 1);
						break;
					}
					else
					{
						NRF_LOG_DEBUG("send data fialed");
					}


				}
				else //不跨界
				{

					
				read_note(read_start_addr, (uint8_t *)tmp_buffer, (upload_cb.note_header.note_len - upload_cb.note_manager_upload_len));  //直接读取剩余的长度
				NRF_LOG_DEBUG("read len %d form addr %x",(upload_cb.note_header.note_len - upload_cb.note_manager_upload_len),read_start_addr);
				if(NRF_SUCCESS == server_send_direct((uint8_t *)tmp_buffer, (upload_cb.note_header.note_len - upload_cb.note_manager_upload_len)))
				{
					upload_cb.note_manager_upload_addr_offset += (upload_cb.note_header.note_len - upload_cb.note_manager_upload_len);
					upload_cb.note_manager_upload_len+=(upload_cb.note_header.note_len - upload_cb.note_manager_upload_len); 
					NRF_LOG_DEBUG("send over");
					upload_cb.note_manager_upload_flag= 0;
					server_notify_host(BLE_CMD_SYNC_IS_END, &end_content, 1);
					break;
				}

				}

			}
			

		}
	}
		
}
void offline_write_flash_poll(void)
{
    if((0x1985 == store_cb.note_header.note_head_flag) && (0 == store_cb.note_manager_sector_erased))
    {
        flash_erase_sector(store_cb.note_manager_note_head_sector);
        while(flash_status_check() & 0x01)
        {
            nrf_delay_ms(10);
        }
        store_cb.note_manager_sector_erased = 1;

    }
	if(0x1985 == store_cb.note_header.note_head_flag)
	{
		if(store_cb.note_manager_write_buffer_index != 0xff)
		{
			if(store_cb.note_manager_valid)  //检查是否有空间写入
			{
				write_note((uint8_t *)&store_cb.note_manager_buffer[store_cb.note_manager_write_buffer_index][0], 256);
				store_cb.note_manager_write_buffer_index = 0xff;
			}

		}
	}
}

uint8_t create_note(void)
{
		
    if(0x1985 != store_cb.note_header.note_head_flag)
    {
		if(p_loacal_device->p_env->device_var_info.stored_total_note_num<MAX_NOTE_ID)
		{
			//init_offline_store_data();
			if(get_free_sector()>0)
			{
				
				store_cb.note_header.note_head_flag = 0x1985;		
				store_cb.note_manager_store_cur_sector = p_loacal_device->p_env->device_var_info.note_read_end_sector;
				p_loacal_device->p_env->device_var_info.note_read_end_sector=get_next_sector(store_cb.note_manager_store_cur_sector,1);
				store_cb.note_header.note_number = p_loacal_device->p_env->device_var_info.stored_total_note_num;
				store_cb.note_manager_note_head_sector = store_cb.note_manager_store_cur_sector;
				store_cb.note_manager_sector_erased = 0;
				upload_cb.note_manager_upload_addr_offset = 256;
				NRF_LOG_DEBUG("create new note num [%d] at [%d] sector tail at sectior[%d]", store_cb.note_header.note_number, store_cb.note_manager_store_cur_sector,p_loacal_device->p_env->device_var_info.note_read_end_sector);							          
				return 0;
			}
			else
			{
				store_cb.note_manager_valid= 0;
				NRF_LOG_DEBUG("no more flash");
				return 1;
			}
		}

    }
    return 1;
}

void offline_note_storage(uint8_t *buf)
{
	st_store_info store_info;
	uint16_t x_pos = 0, y_pos = 0, press = 0;
    memset((uint8_t *)&store_info, 0, sizeof(st_store_info));
	uint8_t    len = 5;
        
        if(0x11 == buf[1]) //如果是按下的
        {  				  
			 create_note();												
            if(pos_solve_cb.note_store_cnt >= 2)  //采集到三次数据包  处理一次
            {
               
                pos_solve_cb.note_store_cnt = 0;
                x_pos = (uint16_t)buf[3] << 8 | buf[2];
                y_pos = (uint16_t)buf[5] << 8 | buf[4];
                press = (uint16_t)buf[7] << 8 | buf[6];
                x_pos = x_pos/2;   //坐标值缩小一半 压力缩小4倍
                y_pos = y_pos/2;
                press = press/4;
                if((pos_solve_cb.store_pos_x != 0) || (pos_solve_cb.store_pos_y != 0))
                {
                    if((pos_solve_cb.store_pos_x == x_pos) && (pos_solve_cb.store_pos_y == y_pos))
                    {
                        return;
                    }
                }
                pos_solve_cb.store_pos_x = x_pos;
                pos_solve_cb.store_pos_y = y_pos;
                if(0 == press)  //如果没有按下 标记按下
                {
                    press = 1;
                }
       
                store_info.store_flag|= 0x7;
						//		NRF_LOG_DEBUG("%x %x %x %x %x",packet->data[0],packet->data[1],packet->data[2],packet->data[3],packet->data[4]);
                store_info.x_l = (uint8_t)(x_pos & 0x00ff);
                store_info.x_h = (uint8_t)((x_pos >> 8) & 0x00ff);
                
                store_info.y_l = (uint8_t)(y_pos & 0x00ff);
                store_info.y_h = (uint8_t)((y_pos >> 8) & 0x001f);  //y的高4位与标识4bit占一个字节
                store_info.press = (uint8_t)(press & 0x00ff);  //压力值

				if(pos_solve_cb.note_store_leave_flag_count != 0)
				{
					store_info.store_flag|= 0x7;
					//		NRF_LOG_DEBUG("%x %x %x %x %x",packet->data[0],packet->data[1],packet->data[2],packet->data[3],packet->data[4]);
					store_info.x_l = 0;
					store_info.x_h = 0;

					store_info.y_l = 0;
					store_info.y_h = 0;  //y的高4位与标识4bit占一个字节
					store_info.press = 0;  //压力值
					pos_solve_cb.note_store_leave_report_flag = 0;
					pos_solve_cb.note_store_leave_flag_count = 0;
				//	NRF_LOG_DEBUG("leave1\r\n");

				}
				else if(!pos_solve_cb.note_store_leave_flag) //press 之后才能报hold点
				{
					pos_solve_cb.note_store_leave_flag = 1;
					pos_solve_cb.note_store_leave_flag_count = 0;
					//NRF_LOG_DEBUG("press\r\n");

				}
				else
				{
					//NRF_LOG_DEBUG("press\r\n");
				}		
								
			
           }
            else
            {
                pos_solve_cb.note_store_cnt++;
                return;
            }
        }
        else if(0x10 == buf[1])  //如果是悬浮
        {
            //leave data
            if(pos_solve_cb.note_store_leave_flag) 
            {
                pos_solve_cb.note_store_leave_report_flag  = 1;              
							
                store_info.store_flag |= 0x7;             
                pos_solve_cb.note_store_cnt = 0;
                //note_store_leave_flag = 0;
                pos_solve_cb.note_store_leave_flag_count ++;
                
                //NRF_LOG_DEBUG2("leave\r\n");
                //NRF_LOG_DEBUG2("count:%d\r\n",note_store_leave_flag_count);

                x_pos = (uint16_t)buf[3] << 8 | buf[2];
                y_pos = (uint16_t)buf[5] << 8 | buf[4];
                press = (uint16_t)buf[7] << 8 | buf[6];
                x_pos = x_pos/2;   //坐标值缩小一半 压力缩小4倍
                y_pos = y_pos/2;
                press = press/4;
                //NRF_LOG_DEBUG2("store_flag:%d\r\n",store_info.store_flag);
                //NRF_LOG_DEBUG2("x:%d\r y:%d\r press:%d\r\n store_x:%d\r store_y:%d\r\n",x_pos,y_pos,press,store_pos_x,store_pos_y);
                if((pos_solve_cb.store_pos_x != 0) || (pos_solve_cb.store_pos_y != 0))
                {
                    if((pos_solve_cb.store_pos_x == x_pos) && (pos_solve_cb.store_pos_y == y_pos))
                    {
                        return;
                    }
                }
                pos_solve_cb.store_pos_x = x_pos;
                pos_solve_cb.store_pos_y = y_pos;

       
                store_info.store_flag|= 0x7;
                        //		NRF_LOG_DEBUG("%x %x %x %x %x",packet->data[0],packet->data[1],packet->data[2],packet->data[3],packet->data[4]);
                store_info.x_l = (uint8_t)(x_pos & 0x00ff);
                store_info.x_h = (uint8_t)((x_pos >> 8) & 0x00ff);
                
                store_info.y_l = (uint8_t)(y_pos & 0x00ff);
                store_info.y_h = (uint8_t)((y_pos >> 8) & 0x001f);  //y的高4位与标识4bit占一个字节
                store_info.press = 0;  //压力值
              //  NRF_LOG_DEBUG("hold\r\n");
                if(pos_solve_cb.note_store_leave_flag_count > 10)
                {
                    pos_solve_cb.note_store_leave_flag_count = 0;
                    pos_solve_cb.note_store_leave_flag = 0;
                    store_info.store_flag|= 0x7;
                    store_info.x_l = 0;
                    store_info.x_h = 0;                    
                    store_info.y_l = 0;
                    store_info.y_h = 0;  //y的高4位与标识4bit占一个字节
                    store_info.press = 0;  //压力值
                  //  NRF_LOG_DEBUG("leave2\r\n");
                    pos_solve_cb.note_store_leave_report_flag  = 0; 

                    
                }
                

                
            }
            else
            {
                
                
                return;
            }
        }
        else
        {
           // if(note_store_leave_flag && note_store_leave_flag_count != 0)
			if(pos_solve_cb.note_store_leave_flag)
            {
                pos_solve_cb.note_store_leave_flag_count = 0;
                pos_solve_cb.note_store_leave_flag = 0;
                store_info.store_flag|= 0x7;
                store_info.x_l = 0;
                store_info.x_h = 0;               
                store_info.y_l = 0;
                store_info.y_h = 0;  //y的高4位与标识4bit占一个字节
                store_info.press = 0;  //压力值
              //  NRF_LOG_DEBUG("leave3\r\n");
                
            }
            else
            {
                return;
            }


        }
//        note_header.note_len += len;  //累计笔记的长度   
        
        if((store_cb.note_manager_read_buffer_offset + len) <= 256)  //如果当前的乒乓buff 能装下
        {
            memcpy((uint8_t *)&store_cb.note_manager_buffer[store_cb.note_manager_read_buffer_index][store_cb.note_manager_read_buffer_offset], (uint8_t *)&store_info, len);
            store_cb.note_manager_read_buffer_offset += len;

            if(store_cb.note_manager_read_buffer_offset >= 256)  //如果要溢出了 换buff
            {
                //buffer full
                store_cb.note_manager_write_buffer_index = store_cb.note_manager_read_buffer_index;
                if(0 == store_cb.note_manager_read_buffer_index)
                {
                    store_cb.note_manager_read_buffer_index = 1;
                }
                else if(1 == store_cb.note_manager_read_buffer_index)
                {
                    store_cb.note_manager_read_buffer_index = 0;
                }
                memset((uint8_t *)&store_cb.note_manager_buffer[store_cb.note_manager_read_buffer_index][0], 0, 256);
                store_cb.note_manager_read_buffer_offset = 0;
            }           
        }
        else  //换buff
        {
            memcpy((uint8_t *)&store_cb.note_manager_buffer[store_cb.note_manager_read_buffer_index][store_cb.note_manager_read_buffer_offset], (uint8_t *)&store_info, (256 - store_cb.note_manager_read_buffer_offset));
            store_cb.note_manager_write_buffer_index = store_cb.note_manager_read_buffer_index;
            if(0 == store_cb.note_manager_read_buffer_index)
            {
                store_cb.note_manager_read_buffer_index = 1;
            }
            else if(1 == store_cb.note_manager_read_buffer_index)
            {
                store_cb.note_manager_read_buffer_index = 0;
            }
            memset((uint8_t *)&store_cb.note_manager_buffer[store_cb.note_manager_read_buffer_index][0], 0, 256);
            memcpy((uint8_t *)&store_cb.note_manager_buffer[store_cb.note_manager_read_buffer_index][0], ((uint8_t *)&store_info + (256 - store_cb.note_manager_read_buffer_offset)), (store_cb.note_manager_read_buffer_offset + len - 256));
            store_cb.note_manager_read_buffer_offset = (store_cb.note_manager_read_buffer_offset + len - 256);
        }
	
	
	
	
	
}
