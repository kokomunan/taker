#ifndef _GET_POSITION_H_
#define _GET_POSITION_H_

#include "stdint.h"


void position_solve_init(void);
void position_solve_offline_callback(uint8_t *buf, uint8_t len);

void position_solve_online_callback(uint8_t *buf, uint8_t len);

#endif

