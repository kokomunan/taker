#ifndef _MMI_H
#define _MMI_H

#include "boards.h"
#include "nordic_common.h"
#include "nrf.h"
#include <stdbool.h>

extern void power_button_long_press(void);
extern void power_button_short_release(void);
extern void power_button_double_press(void);


#define GPIO_GET_PIN(pin_no) nrf_gpio_pin_read(pin_no)
#define SETBIT(BYTE,BIT)         ( (BYTE) |= (uint16_t)((uint16_t)1<<(uint16_t)(BIT)) )
#define CLRBIT(BYTE,BIT)         ( (BYTE) &= (uint16_t)((uint16_t)((uint16_t)1<<(uint16_t)(BIT))^(uint16_t)0xffff) )

#define SHORT_PRESS_TIME		      5
#define DOUBLE_PRESS_INTERVAL        50
#define LONG_PRESS_TIME				100
#define VERY_LONG_PRESS_TIME		300


typedef enum _ButtonsNumber
{
   BTN_PWR=0,
   BTN_TOTAL_NO,
}ButtonsNumber;

typedef enum buttonevents
{
   NO_BUTTON_EVENT,
   SHORT_PRESS,
   DOUBLE_PRESS,
   LONG_PRESS,
   VERY_LONG_PRESS,
   KEEP_PRESS
}Buttonevents;

enum KEY_EVENT{
    
    NON_KEY,
    CONFIRM_KEY,
    RETURN_KEY,
    FORWAD_KEY,
    BACK_KEY,
    
    
};
typedef struct buttonstatus
{
   uint8_t StartEventDetect;
   uint8_t pinStatus;
   uint8_t pinLastStatus;
   uint8_t short_press_counts;
   uint8_t short_press_interval;
   uint16_t buttonPressedTimeslots;
   uint16_t LongPressThreshold;   
   void (* press_callback)(void);
   void (* short_release_callback)(void);
   void (* double_press_callback)(void);
   void (* long_release_callback)(void);
   void (* keep_press_callback)(void);
}PinStatus;

void ButtonProcessLoop(void);
static void ButtonEventProcess(void);
void mmi_init(void);
static Buttonevents CheckButtonEvent(ButtonsNumber buttonNO);
static void NULLFunction(void);
void button_init(void);
#endif

