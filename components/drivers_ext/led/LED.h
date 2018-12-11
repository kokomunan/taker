#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <stdbool.h>
#include "nrf_gpio.h"
#include "boards.h"

#ifndef NULL
#define NULL 0x0000
#endif

typedef enum UIdisplay
{
/*0*/   UI_NONE = 0,
        UI_PAIRING,
        UI_CONNECTED,
        UI_RECONNECTING,
        UI_OTA,
        UI_UPLOAD,
        //EVENT
        UI_BATTERY_LOW,
        UI_NOTE_STORE,
        UI_COUNTS
}UIDisplayType ;

#define RED_LED_ON()    	nrf_gpio_pin_clear(RED_LED);
#define RED_LED_OFF()	    nrf_gpio_pin_set(RED_LED);

#define GREEN_LED_ON()		nrf_gpio_pin_clear(GREEN_LED);
#define GREEN_LED_OFF()		nrf_gpio_pin_set(GREEN_LED);

#define BLUE_LED_ON()		nrf_gpio_pin_clear(BLUE_LED);
#define BLUE_LED_OFF()		nrf_gpio_pin_set(BLUE_LED);

typedef struct LEDpara 
{
	unsigned long m_u32Interval;    			//uint 10ms //light off time
	unsigned long m_u32LastTime; 			//unit 10ms     //light on time    

} LEDPara;

typedef struct LEDAction
{
	LEDPara* p_RedLedPara;
	LEDPara* p_BlueLedPara;
	LEDPara* p_GreenLedPara;
	
	uint8_t m_u16RedParaNum;
	uint8_t m_u16BlueParaNum;
	uint8_t m_u16GreenParaNum;
	
	uint8_t m_u16RedRepeatMode:1; //0: no repeat   1:repeat
	uint8_t m_u16BlueRepeatMode:1; //0: no repeat   1:repeat
	uint8_t m_u16GreenRepeatMode:1; //0: no repeat   1:repeat

	uint8_t m_u16RedRepeatCount;
	uint8_t m_u16BlueRepeatCount;
	uint8_t m_u16GreenRepeatCount;
		
	uint8_t m_u16RedHeadCount;
	uint8_t m_u16BlueHeadCount;
	uint8_t m_u16GreenHeadCount;
}LEDAction;


void led_init(void);
bool led_start(UIDisplayType led_display);
int  led_stop(void);
bool led_insert(UIDisplayType led_display);

#endif

