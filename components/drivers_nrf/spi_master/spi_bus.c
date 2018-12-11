

#include "spi_bus.h"
#include "nrf_error.h"
#include "boards.h"
#include "ble_debug_assert_handler.h"
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define SPI_INSTANCE  0
static const nrf_drv_spi_t spi0 = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */

static volatile bool spi_xfer_done; 


/**
 * @brief SPI user event handler.
 * @param event
 */
void spi_event_handler(nrf_drv_spi_evt_t const * p_event)
{
    spi_xfer_done = true;
    NRF_LOG_INFO("Transfer completed.\r\n");

}
void spi_master_init(void)
{
   
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = SPI_CS;
    spi_config.miso_pin = SPI_MISO;
    spi_config.mosi_pin = SPI_MOSI;
    spi_config.sck_pin  = SPI_CLK;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi0, &spi_config, spi_event_handler));
              
}

void spi_send_recv(uint8_t * p_tx_data, uint8_t * p_rx_data, uint16_t len)
{
    uint32_t err_code;
    // Start transfer.
    spi_xfer_done = false;
    if(p_rx_data == NULL)
    {
        err_code = nrf_drv_spi_transfer(&spi0, p_tx_data, len, p_rx_data, 0);
    }
    else
    {
        err_code = nrf_drv_spi_transfer(&spi0, p_tx_data, len, p_rx_data, len);
    }
  while (!spi_xfer_done)
    {
       
    }
    APP_ERROR_CHECK(err_code);
}

