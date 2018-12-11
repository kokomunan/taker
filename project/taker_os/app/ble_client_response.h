#ifndef _BLE_CLIENT_RESPONSE_H
#define _BLE_CLIENT_RESPONSE_H
#include "main_deamon.h"


#include "stdint.h"
void ble_client_init(st_device *p_device);
void ble_client_response(void * p_event_data, uint16_t event_size);

#endif
