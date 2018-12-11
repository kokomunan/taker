#include "LED.h"
#include "app_util.h"
#include "boards.h"
#include "nrf_delay.h"
#include "app_timer.h"
#include "nrf_log.h"
#define LED_EVENT_DEEP   3
static  unsigned long s_u32RedPreTime = 0;
static  unsigned long s_u32GreenPreTime = 0;
static  unsigned long s_u32BluePreTime = 0;
static  unsigned long s_u32CurTime = 0;
static  LEDAction* s_LedAction = NULL;
static  uint8_t u8RedCount = 0;
static  uint8_t u8GreenCount = 0;
static  uint8_t u8BlueCount = 0;
static  uint8_t u8RedRepeatCount = 0;
static  uint8_t u8GreenRepeatCount = 0;
static  uint8_t u8BlueRepeatCount = 0;
static  uint8_t u8RedStatus= 0;// 0: low 1:high
static  uint8_t u8GreenStatus = 0;
static  uint8_t u8BlueStatus = 0;
static  uint8_t u8Start = 0;
static  uint8_t u8EndFlag = 0;
static uint8_t LEDTick = 0; //LSB is 50ms

static uint8_t current_ui_display = 0;
static uint8_t led_event_dis=0;
static uint8_t led_event_ui[LED_EVENT_DEEP];
static uint8_t led_pre_ui=0;
void  led_reload_ui(uint8_t led_display);
APP_TIMER_DEF(led_timer_id);   
void led_timer_callback(void * p_context)
{
	
	LEDTick++;		
	if(u8Start == 1)
	{
		s_u32CurTime = LEDTick;	
		if((u8RedCount < s_LedAction->m_u16RedParaNum)&&(s_LedAction->m_u16RedParaNum > 0))
		{
			if((s_u32CurTime -s_u32RedPreTime >= s_LedAction->p_RedLedPara[u8RedCount].m_u32Interval)&&(u8RedStatus == 0))
			{
				RED_LED_ON();
				u8RedStatus = 1;
				s_u32RedPreTime = s_u32CurTime;				
			}
			else if((s_u32CurTime -s_u32RedPreTime >= s_LedAction->p_RedLedPara[u8RedCount].m_u32LastTime)&&(u8RedStatus ==1))
			{
				
				RED_LED_OFF();
				u8RedStatus = 0;
				s_u32RedPreTime = s_u32CurTime;
				u8RedCount ++;
				if((s_LedAction->m_u16RedRepeatMode == 1)&&(u8RedCount >= s_LedAction->m_u16RedParaNum))//one repeat cycle complete
				{
					if(0 == s_LedAction->m_u16RedRepeatCount) //Nonstop repeat
					{
						//continue repeat
						u8RedCount = s_LedAction->m_u16RedHeadCount;							
					}
					else
					{
						u8RedRepeatCount ++;						
						
						if(u8RedRepeatCount < s_LedAction->m_u16RedRepeatCount) //repeat
						{
							//continue repeat
							u8RedCount = s_LedAction->m_u16RedHeadCount;							
						}
						else
						{
						    if(u8EndFlag>0)
							{
								u8EndFlag--;
							}
						}
					}
					
				}
			}
		}

		if((u8GreenCount < s_LedAction->m_u16GreenParaNum)&&(s_LedAction->m_u16GreenParaNum > 0))
		{
			if((s_u32CurTime -s_u32GreenPreTime >= s_LedAction->p_GreenLedPara[u8GreenCount].m_u32Interval)&&(u8GreenStatus == 0))
			{
				GREEN_LED_ON();
				u8GreenStatus = 1;
				s_u32GreenPreTime = s_u32CurTime;
			}
			else if((s_u32CurTime -s_u32GreenPreTime >= s_LedAction->p_GreenLedPara[u8GreenCount].m_u32LastTime)&&(u8GreenStatus == 1))
			{
				GREEN_LED_OFF();
				u8GreenStatus = 0;
				s_u32GreenPreTime = s_u32CurTime;
				
				u8GreenCount ++;
				if((s_LedAction->m_u16GreenRepeatMode == 1)&&(u8GreenCount >= s_LedAction->m_u16GreenParaNum))
				{
					if(0 == s_LedAction->m_u16GreenRepeatCount) //Nonstop repeat
					{
						//continue repeat
						u8GreenCount = s_LedAction->m_u16GreenHeadCount;							
					}
					else
					{
						u8GreenRepeatCount ++;						
						
						if(u8GreenRepeatCount <=s_LedAction->m_u16GreenRepeatCount) //repeat
						{
							//continue repeat
							u8GreenCount = s_LedAction->m_u16GreenHeadCount;							
						}
						else
						{
							if(u8EndFlag>0)
							{
								u8EndFlag--;
							}
						}
					}
				}
			}
		}

		if((u8BlueCount < s_LedAction->m_u16BlueParaNum)&&(s_LedAction->m_u16BlueParaNum > 0))
		{
			if((s_u32CurTime -s_u32BluePreTime >= s_LedAction->p_BlueLedPara[u8BlueCount].m_u32Interval)&&(u8BlueStatus == 0))
			{
				BLUE_LED_ON();
				u8BlueStatus = 1;
				s_u32BluePreTime = s_u32CurTime;
			}
			else if((s_u32CurTime -s_u32BluePreTime >= s_LedAction->p_BlueLedPara[u8BlueCount].m_u32LastTime)&&(u8BlueStatus == 1))
			{
				BLUE_LED_OFF();
				u8BlueStatus = 0;
				s_u32BluePreTime = s_u32CurTime;
				
				u8BlueCount ++;
				if((s_LedAction->m_u16BlueRepeatMode == 1)&&(u8BlueCount >= s_LedAction->m_u16BlueParaNum))
				{
					if(0 == s_LedAction->m_u16BlueRepeatCount) //Nonstop repeat
					{
						//continue repeat
						u8BlueCount = s_LedAction->m_u16BlueHeadCount;							
					}
					else
					{
						u8BlueRepeatCount ++;						
						
						if(u8BlueRepeatCount < s_LedAction->m_u16BlueRepeatCount) //repeat
						{
							//continue repeat
							u8BlueCount = s_LedAction->m_u16BlueHeadCount;							
						}
						else
						{
						    //blue repeat end
							if(u8EndFlag>0)
							{
								u8EndFlag--;
							}
						}
					}
				}
			}
			
		}
		if(u8EndFlag == 0)
		{
            int8_t i;
            if(led_event_ui[0]==0)
            {
                 led_event_dis=0;
                 led_reload_ui(led_pre_ui);
            }
            else
            {
                                    
                for(i=LED_EVENT_DEEP;i>0;i--)
                {
                    if(led_event_ui[i-1]!=0)
                    {
                        led_reload_ui(led_event_ui[i]);
                        led_event_ui[i-1]=0;                   
                    }
                }               
             
            }              
		
		}
	}
    else
    {
        RED_LED_OFF();
        BLUE_LED_OFF();  
    }

	
	
		
}

//Pairing
/***********************************************************************************/

static const LEDPara pairing_red[] = {                              {50,50}  ,            //white led1 on                      
                                                                    {50,50}              //white led1 on                      
};
                                                        
static const LEDPara pairing_blue[] = {
                                                                    {0,50}  ,            //white led1 on                      
                                                                    {50,50}              //white led1 on                      
};
static const LEDAction pairing = {
								(LEDPara*)pairing_red,				 //p_BlueLedPara
								(LEDPara*)pairing_blue,				 //p_BlueLedPara
								NULL,				 //p_GreenLedPara
								
								2,					 //m_u16RedParaNum
								2,					 //m_u16BlueParaNum
								0,					 //m_u16GreenParaNum
								
								1,					 //m_u16RedRepeatMode
								1,					 //m_u16BlueRepeatMode
								0,					 //m_u16GreenRepeatMode

								0,					 //m_u16RedRepeatCount
								0,					 //m_u16BlueRepeatCount // 0 = always play
								0,					 //m_u16GreenRepeatCount
								
								1,    				 //m_u16RedHeadCount
								1,					 //m_u16BlueHeadCount  //
								0,					 //m_u16GreenHeadCount
						   };

//Reconnecting
/***********************************************************************************/
static const LEDPara reconnecting_blue[] = { 
                                                {10,10},
                                                {10,10},
                                                {190,10},
						   	     };
static const LEDAction reconnecting = {
								NULL,  //p_RedLedPara
								(LEDPara*)reconnecting_blue,				 //p_BlueLedPara
								NULL,				 //p_GreenLedPara
								
								0,					 //m_u16RedParaNum
								3,					 //m_u16BlueParaNum
								0,					 //m_u16GreenParaNum
								
								0,					 //m_u16RedRepeatMode
								1,					 //m_u16BlueRepeatMode
								0,					 //m_u16GreenRepeatMode

								0,					 //m_u16RedRepeatCount
								0,					 //m_u16BlueRepeatCount
								0,					 //m_u16GreenRepeatCount
								
								1,    				 //m_u16RedHeadCount
								1,					 //m_u16BlueHeadCount
								1,					 //m_u16GreenHeadCount
						   };


/***********************************************************************************/

static const LEDPara connected_blue[] = {
                                                {2,0xffffffff}              //white led1 on                      
};

static const LEDAction ble_connected =  {
								NULL,  //p_RedLedPara
								(LEDPara*)connected_blue, //p_BlueLedPara
								NULL,  //p_RedLedPara
								
								0,					 //m_u16RedParaNum
								1,					 //m_u16BlueParaNum
								0,					 //m_u16GreenParaNum
								
								0,					 //m_u16RedRepeatMode
								1,					 //m_u16BlueRepeatMode
								0,					 //m_u16GreenRepeatMode

								0,					 //m_u16RedRepeatCount
								0,					 //m_u16BlueRepeatCount
								0,					 //m_u16GreenRepeatCount
								
								0,    				 //m_u16RedHeadCount
								0,					 //m_u16BlueHeadCount
								0,					 //m_u16GreenHeadCount
						   };    
/***********************************************************************************/						   
static const LEDPara ota_blue[] = {
                                                            {2,0xffffffff},              //white led1 on                      
                                                        };
static const LEDPara ota_red[] = {
                                                            {2,0xffffffff},              //white led1 on                      
                                                        };

static const LEDAction ble_ota =  {
								(LEDPara*)ota_red,  //p_RedLedPara
								(LEDPara*)ota_blue, //p_BlueLedPara
								NULL,  //p_RedLedPara
								
								1,					 //m_u16RedParaNum
								1,					 //m_u16BlueParaNum
								0,					 //m_u16GreenParaNum
								
								1,					 //m_u16RedRepeatMode
								1,					 //m_u16BlueRepeatMode
								0,					 //m_u16GreenRepeatMode

								0,					 //m_u16RedRepeatCount
								0,					 //m_u16BlueRepeatCount
								0,					 //m_u16GreenRepeatCount
								
								0,    				 //m_u16RedHeadCount
								0,					 //m_u16BlueHeadCount
								0,					 //m_u16GreenHeadCount
						   }; 


/***********************************************************************************/

static const LEDPara upload_blue[] = {
                                                            {2,0xffffffff},              //white led1 on                      
                                                        };
static const LEDPara upload_red[] = {
                                                            {2,0xffffffff},              //white led1 on                      
                                                        };

static const LEDAction upload =  {
								(LEDPara*)upload_blue,  //p_RedLedPara
								(LEDPara*)upload_red, //p_BlueLedPara
								NULL,  //p_RedLedPara
								
								1,					 //m_u16RedParaNum
								1,					 //m_u16BlueParaNum
								0,					 //m_u16GreenParaNum
								
								1,					 //m_u16RedRepeatMode
								1,					 //m_u16BlueRepeatMode
								0,					 //m_u16GreenRepeatMode

								0,					 //m_u16RedRepeatCount
								0,					 //m_u16BlueRepeatCount
								0,					 //m_u16GreenRepeatCount
								
								0,    				 //m_u16RedHeadCount
								0,					 //m_u16BlueHeadCount
								0,					 //m_u16GreenHeadCount
						   }; 


/***********************************************************************************/                           
                           
                           
                           
                           
                           
                           
                           
static const LEDPara battery_low_red[] = {
                                                {20, 20} ,
                                                //{50, 100},
                                                //{5, 10},
                                           };
                                                        

static const LEDAction battery_low =  {
								(LEDPara*)battery_low_red,  //p_RedLedPara
                                NULL,  //p_BlueLedPara								
								NULL, //p_GreenLedPara
								
								1,					 //m_u16RedParaNum
								0,					 //m_u16BlueParaNum
								0,					 //m_u16GreenParaNum
								
								1,					 //m_u16RedRepeatMode
								0,					 //m_u16BlueRepeatMode
								0,					 //m_u16GreenRepeatMode

								3,					 //m_u16RedRepeatCount
								0,					 //m_u16BlueRepeatCount
								0,					 //m_u16GreenRepeatCount
								
								0,    				 //m_u16RedHeadCount
								0,					 //m_u16BlueHeadCount
								0,					 //m_u16GreenHeadCount
						   };                                        
/***********************************************************************************/
static const LEDPara note_store_red[] = {
                                                {30, 30} ,
                                           };
static const LEDPara note_store_blue[] = {
                                                {30, 30} ,
                                           };                                                        

static const LEDAction note_store =  {
								(LEDPara*)note_store_red,  //p_RedLedPara
                                (LEDPara*)note_store_blue,  //p_BlueLedPara								
								NULL, //p_GreenLedPara
								
								1,					 //m_u16RedParaNum
								1,					 //m_u16BlueParaNum
								0,					 //m_u16GreenParaNum
								
								1,					 //m_u16RedRepeatMode
								1,					 //m_u16BlueRepeatMode
								0,					 //m_u16GreenRepeatMode

								2,					 //m_u16RedRepeatCount
								2,					 //m_u16BlueRepeatCount
								0,					 //m_u16GreenRepeatCount
								
								0,    				 //m_u16RedHeadCount
								0,					 //m_u16BlueHeadCount
								0,					 //m_u16GreenHeadCount
						   }; 

                 

                           
/***********************************************************************************/
//Command list
/***********************************************************************************/
static const LEDAction * const CommandList[UI_COUNTS] =   {
														NULL, //UI_poweron
														&pairing, //UI_poweron
														&ble_connected, //UI_poweron
														&reconnecting,
														&ble_ota,
														&upload,
														&battery_low,
														&note_store,
													};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//LSB is 50ms
void led_init(void)
{
    s_u32RedPreTime = 0;
    s_u32GreenPreTime = 0;
    s_u32BluePreTime = 0;
    s_u32CurTime = 0;

    u8RedCount = 0;
    u8GreenCount = 0;
    u8BlueCount = 0;
    u8RedRepeatCount = 0;
    u8GreenRepeatCount = 0;
    u8BlueRepeatCount = 0;
    u8RedStatus= 0;// 0: low 1:high
    u8GreenStatus = 0;
    u8BlueStatus = 0;
    u8Start = 0;
    u8EndFlag = 0;
    LEDTick = 0; 
    current_ui_display = 0;
    led_event_dis=0; 
    for(uint8_t i=0;i<LED_EVENT_DEEP;i++)
    {
        led_event_ui[i]=0; 
    }
        
    RED_LED_OFF();
	BLUE_LED_OFF(); 
    GREEN_LED_OFF();
   app_timer_create(&led_timer_id,APP_TIMER_MODE_REPEATED,led_timer_callback);//创建定时检测电量定时器  
   app_timer_start(led_timer_id, 50, NULL); //创建50 毫秒定时器     

       
}
int led_stop(void)
{
	u8Start = 0;
	RED_LED_OFF();
	BLUE_LED_OFF();    
	current_ui_display = UI_NONE;
	return 0;
}
bool led_start(UIDisplayType led_display)
{
	
	NRF_LOG_DEBUG("start led ui%d\r\n",led_display);
    if(led_display>UI_BATTERY_LOW)
    {
        return false;
    }
	
	if(led_event_dis==0)  //没有任何显示事件
	{

		if(current_ui_display == led_display)
		{
			return false;
		}

        led_stop();
		if(CommandList[led_display] != NULL)
		{
			s_LedAction = (LEDAction*)CommandList[led_display]; 
			s_u32RedPreTime = LEDTick;
			s_u32GreenPreTime = LEDTick;
			s_u32BluePreTime = LEDTick;

			u8RedCount = 0;
			u8GreenCount = 0;
			u8BlueCount = 0;

			u8RedRepeatCount = 0;
			u8GreenRepeatCount = 0;
			u8BlueRepeatCount = 0;

			u8RedStatus= 0;
			u8GreenStatus = 0;
			u8BlueStatus = 0;
			u8Start = true;
			current_ui_display = led_display;   //如果此时只有非事件显示  直接切换新的UI
			u8EndFlag = 4;
			
			return true;
		}
		return false;
	}
	else
	{	 
		if(CommandList[led_display] != NULL)
		{
             led_pre_ui=led_display;  //等待事件都显示完毕后 从pre->curunt  切换	
			return true;
		}
		else
		{
			return false;
		}
	}
}
bool led_insert(UIDisplayType led_display)
{
	
    if((led_display>=UI_COUNTS) || (led_display < UI_BATTERY_LOW))
    {
        return false;
    }
    
    if(CommandList[led_display] != NULL)
    {
     
        uint8_t i=0;
        for(i=0;i<3;i++)
        {
            if(led_event_ui[i]==0)
            {
                led_event_ui[i] = led_display; 
            }
        }
        if(i==0)
        {
            led_stop();
            s_LedAction = (LEDAction*)CommandList[led_display]; 
            s_u32RedPreTime = LEDTick;
            s_u32GreenPreTime = LEDTick;
            s_u32BluePreTime = LEDTick;

            u8RedCount = 0;
            u8GreenCount = 0;
            u8BlueCount = 0;

            u8RedRepeatCount = 0;
            u8GreenRepeatCount = 0;
            u8BlueRepeatCount = 0;

            u8RedStatus= 0;
            u8GreenStatus = 0;
            u8BlueStatus = 0;
            u8Start = true;
            current_ui_display = led_display;           

        }            
        led_event_dis=1;
        return true;
    }
    else
    {
        return false; 
    }
   

}

void  led_reload_ui(uint8_t led_display)
{
        
    led_stop() ;
    s_LedAction = (LEDAction*)CommandList[led_display]; 
    s_u32RedPreTime = LEDTick;
    s_u32GreenPreTime = LEDTick;
    s_u32BluePreTime = LEDTick;

    u8RedCount = 0;
    u8GreenCount = 0;
    u8BlueCount = 0;

    u8RedRepeatCount = 0;
    u8GreenRepeatCount = 0;
    u8BlueRepeatCount = 0;

    u8RedStatus= 0;
    u8GreenStatus = 0;
    u8BlueStatus = 0;
    u8Start = true;
    current_ui_display = led_display;    
    
    
    
}
