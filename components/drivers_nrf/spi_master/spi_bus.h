#ifndef __SPI_BUS_H_
#define __SPI_BUS_H_

#include <stdint.h>
#include <stdbool.h>
#include "nrf_drv_spi.h"


void spi_master_init(void);
void spi_send_recv(uint8_t * p_tx_data, uint8_t * p_rx_data, uint16_t len);

#endif

