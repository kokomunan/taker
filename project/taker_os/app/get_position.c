#include "get_position.h"
#include "jedi_device.h"
#include "server_thread.h"
#include "NRF_LOG.h"
#include "note_manager.h"

stPosBuff g_position_buffer;
void position_solve_init(void)
{
    
  memset((uint8_t *)&g_position_buffer, 0, sizeof(stPosBuff));
}


void position_solve_offline_callback(uint8_t *buf, uint8_t len)
{

  offline_note_storage(buf);
}

void position_solve_online_callback(uint8_t *buf, uint8_t len)
{
    uint8_t rst;
    if(0 == g_position_buffer.data_count)
    {
       
        memcpy(&g_position_buffer.pos_data[0], (uint8_t *)buf, len);
        if(buf[1] != 0x00)
        {
            g_position_buffer.data_count++;
        }
        else
        {
            rst = server_notify_host(BLE_CMD_POS_DATA, (uint8_t *)&g_position_buffer.pos_data[0], 8);
            if(false == rst)
            {
               NRF_LOG_ERROR("send fifo overflaow");
            }
            g_position_buffer.data_count = 0;
        }
    }
    else
    {
        memcpy(&g_position_buffer.pos_data[1], (uint8_t *)buf, len);
        
        rst = server_notify_host(BLE_CMD_POS_DATA, (uint8_t *)&g_position_buffer.pos_data[0], 16);
        if(false == rst)
        {
           NRF_LOG_ERROR("send fifo overflaow");
        }
        g_position_buffer.data_count = 0;
    }
    
    
}
