#include "ext_flash.h"
#include "spi_bus.h"
#include "boards.h"
#include "nrf_log.h"

uint8_t  flash_hw_check(void)
{
   
    uint8_t send_buffer[6];
    uint8_t rcv_buffer[6];
    //write enable
    send_buffer[0] = FLASH_WAKE_UP;
    send_buffer[1] = 0;
    send_buffer[2] = 0;
    send_buffer[3] = 0;
    send_buffer[4] = 0;
		
    nrf_gpio_pin_clear(SPI_CS);
    spi_send_recv(send_buffer, rcv_buffer, 5);
    nrf_gpio_pin_set(SPI_CS);
    if(FLASH_DID == rcv_buffer[4])
    {
        //g_main_status.flash_store_data_flag = 1;
		return 0;
    }
    else
    {
        
       NRF_LOG_INFO("flash has errors\r\n");
		return 1;
    }
}

uint8_t flash_status_check(void)
{
    uint8_t send_buffer[2];
    uint8_t rcv_buffer[2];
    //write enable
    send_buffer[0] = FLASH_READ_STATUS;   
    nrf_gpio_pin_clear(SPI_CS);
    spi_send_recv(send_buffer, rcv_buffer, 2);
    nrf_gpio_pin_set(SPI_CS);
    return rcv_buffer[1];
}

void flash_write_enable(void)
{
    uint8_t send_buffer[1];
    //write enable
    send_buffer[0] = FLASH_WRITE_ENABLE;    
    nrf_gpio_pin_clear(SPI_CS);
    spi_send_recv(send_buffer, NULL, 1);
    nrf_gpio_pin_set(SPI_CS);
}
//erase one sectot about 80ms in  real test
void flash_erase_sector(uint16_t sector_num)
{
    uint8_t send_buffer[4];//, read_buffer[4];
    uint32_t WriteAddr = sector_num * FLASH_SECTOR_SIZE;
    
    flash_write_enable();
    //earse
    send_buffer[0] = FLASH_SECTOR_ERASE;
    send_buffer[1] = (uint8_t)((WriteAddr&0x00ff0000)>>16);
    send_buffer[2] = (uint8_t)((WriteAddr&0x0000ff00)>>8);
    send_buffer[3] = (uint8_t)WriteAddr;
    
    nrf_gpio_pin_clear(SPI_CS);
    spi_send_recv(send_buffer, NULL, 4);    
    nrf_gpio_pin_set(SPI_CS);
	NRF_LOG_DEBUG("erase flash sector%d",sector_num);
}
void flash_enter_power_down()
{
   
    uint8_t send_buffer[1];
    //write enable
    send_buffer[0] = FLASH_DEEP_POWER_DOWN;   
    nrf_gpio_pin_clear(SPI_CS);
    spi_send_recv(send_buffer, NULL, 1);
    nrf_gpio_pin_set(SPI_CS);
    
}
//from the sent over spi data, flash will return standby mode use 20us in spec
void flash_wake_up(void)
{
  
    uint8_t send_buffer[1];
    //write enable
    send_buffer[0] = FLASH_WAKE_UP;
    
    nrf_gpio_pin_clear(SPI_CS);
    spi_send_recv(send_buffer, NULL, 1);
    nrf_gpio_pin_set(SPI_CS);
    
}

