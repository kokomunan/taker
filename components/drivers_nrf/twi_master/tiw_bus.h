#ifndef _TIW_BUS_H
#define _TIW_BUS_H

#include "stdint.h"
#include "boards.h"
#include "nrf_drv_twi.h"


uint8_t tiw_bus_init(void);
ret_code_t twi_master_tx ( uint8_t               address,uint8_t const *       p_data, uint8_t               length,bool                  no_stop);

ret_code_t twi_master_rx(uint8_t  address,uint8_t *             p_data, uint8_t               length);

#endif
