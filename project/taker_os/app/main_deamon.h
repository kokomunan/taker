#ifndef MAIN_DEAMON_H__
#define MAIN_DEAMON_H__
#include "stdint.h"
#include "env.h"
typedef struct
{
    st_env  *p_env;
    
    uint8_t status;
    
    
    
    
    
}st_device;

void mian_deamon_thread (void * arg);
st_device*  get_device_info(void);

#endif

