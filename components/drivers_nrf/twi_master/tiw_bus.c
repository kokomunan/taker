
#include "tiw_bus.h"

static const nrf_drv_twi_t m_twi_master = NRF_DRV_TWI_INSTANCE(MASTER_TWI_INST);

uint8_t tiw_bus_init(void)
{
    
    ret_code_t ret;
    const nrf_drv_twi_config_t config =
    {
       .scl                = I2C_SCL,
       .sda                = I2C_SDA,
       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = TWI_DEFAULT_CONFIG_IRQ_PRIORITY,
       .clear_bus_init     = false
    };

    ret = nrf_drv_twi_init(&m_twi_master, &config, NULL, NULL);

    if (NRF_SUCCESS == ret)
    {
        nrf_drv_twi_enable(&m_twi_master);
    }

    return ret;
}


ret_code_t twi_master_tx ( uint8_t               address,uint8_t const *       p_data, uint8_t               length,bool                  no_stop)
{
    
   return nrf_drv_twi_tx(&m_twi_master,address,p_data,length,no_stop);
    
    
    
}

ret_code_t twi_master_rx(uint8_t  address,uint8_t *             p_data, uint8_t               length)
{
    
  return nrf_drv_twi_rx(&m_twi_master,address,p_data,length);
       
}
