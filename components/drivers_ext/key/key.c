#include <stdint.h>
#include <string.h>
#include "key.h"

const uint32_t ButtonArray[BTN_TOTAL_NO] = {BUTTON_DET};
const uint32_t ButtonArray_polar[BTN_TOTAL_NO]={1};
const uint32_t ButtonArray_double_interval[BTN_TOTAL_NO]={DOUBLE_PRESS_INTERVAL};
static uint16_t button_status = 0;
static PinStatus ButtonStatusArray[BTN_TOTAL_NO] = {0};
bool GetButtonStatus(ButtonsNumber buttonNO)//return a false when button pressed
{
	return (bool)(((1<<buttonNO) & button_status));
}

void ButtonProcessLoop(void)
{
    uint8_t BIndex = 0;
    
    for(BIndex = 0;BIndex<BTN_TOTAL_NO;BIndex++)
    {
        if(ButtonArray_polar[BIndex] == GPIO_GET_PIN(ButtonArray[BIndex]))  //读取io
        {
            SETBIT(button_status,BIndex);  //如果按下了 设置位
         
        }	
        else
        {
            CLRBIT(button_status,BIndex);   //如果没有按下就清除位
        				
        }
    }
  
    ButtonEventProcess();	
}
static void NULLFunction(void)
{
	return ;
}
void button_init(void)
{
    uint8_t i = 0;
    for(i = 0;i<BTN_TOTAL_NO;i++)
    {	
    	ButtonStatusArray[i].press_callback = &NULLFunction;
    	ButtonStatusArray[i].short_release_callback = &NULLFunction;
    	ButtonStatusArray[i].long_release_callback = &NULLFunction;
    	ButtonStatusArray[i].keep_press_callback = &NULLFunction;
		ButtonStatusArray[i].double_press_callback=&NULLFunction;
		ButtonStatusArray[i].LongPressThreshold = LONG_PRESS_TIME;  //长按的时间
		ButtonStatusArray[i].short_press_counts = 0;
		ButtonStatusArray[i].short_press_interval = 0;
		CLRBIT(button_status,i);        	        
		ButtonStatusArray[i].pinStatus = GetButtonStatus((ButtonsNumber)i); //记录状态引脚
		ButtonStatusArray[i].pinLastStatus = ButtonStatusArray[i].pinStatus; //记录上次引脚状态
		ButtonStatusArray[i].StartEventDetect = false;
    }	

				
    ButtonStatusArray[BTN_PWR].long_release_callback = &power_button_long_press;
    ButtonStatusArray[BTN_PWR].short_release_callback = &power_button_short_release;
    ButtonStatusArray[BTN_PWR].double_press_callback = &power_button_double_press;
				

}


static void ButtonEventProcess(void)
{
    uint8_t i = 0;
    for(i = 0;i<BTN_TOTAL_NO;i++)
    {
        CheckButtonEvent((ButtonsNumber)i);
    }			
}

static Buttonevents CheckButtonEvent(ButtonsNumber buttonNO)
{
    ButtonStatusArray[buttonNO].pinStatus = GetButtonStatus(buttonNO); //得到当前引脚状态

   if((0 == ButtonStatusArray[buttonNO].StartEventDetect)
      &&(BUTTONS_ACTIVE_STATE == ButtonStatusArray[buttonNO].pinStatus))//如果按下了 并且没有检测开始
     {
         ButtonStatusArray[buttonNO].pinLastStatus = ButtonStatusArray[buttonNO].pinStatus ;  //更新状态
         return NO_BUTTON_EVENT;  //无事件
      }

   if(BUTTONS_ACTIVE_STATE == ButtonStatusArray[buttonNO].pinStatus) //如果按键按下
   {
      if(ButtonStatusArray[buttonNO].pinStatus != ButtonStatusArray[buttonNO].pinLastStatus) //按键状态发生变化
      {                  
        ButtonStatusArray[buttonNO].buttonPressedTimeslots = 0;  //按下的时间清零
		
      }
      else //如果之前也是按下 现在也是按下
      {
         
         ButtonStatusArray[buttonNO].buttonPressedTimeslots ++; //持续时间累积
         if(ButtonStatusArray[buttonNO].LongPressThreshold < ButtonStatusArray[buttonNO].buttonPressedTimeslots) //如果按下的累计时间超过200tick
         {
  
            ButtonStatusArray[buttonNO].long_release_callback();     //回调长按   

         }         
      }
   }
   else // 如果按键释放了
   {
      if(ButtonStatusArray[buttonNO].pinStatus != ButtonStatusArray[buttonNO].pinLastStatus) //按键状态变化  之前是按下的
      {
         if((ButtonStatusArray[buttonNO].buttonPressedTimeslots < ButtonStatusArray[buttonNO].LongPressThreshold)  
         &&(ButtonStatusArray[buttonNO].buttonPressedTimeslots > SHORT_PRESS_TIME))// 如果案件按下的时间大于短按的时间 但小于长按的
         {
            ButtonStatusArray[buttonNO].buttonPressedTimeslots = 0;  //按下的时间清零
            ButtonStatusArray[buttonNO].short_press_counts ++;  //按下事件累加
            ButtonStatusArray[buttonNO].short_press_interval = 0; //短按时间清零
            if(ButtonStatusArray[buttonNO].short_press_counts > 1)  //如果大于1 说明上次发生过一次
            {
                ButtonStatusArray[buttonNO].short_press_counts = 0; //单机累计书清零
                ButtonStatusArray[buttonNO].double_press_callback();  //回调双击事件       
            }
           
         } 
				 
      }
      else  //按键没有变化 之前也是释放的
      {
         //start next button event detecting
         ButtonStatusArray[buttonNO].StartEventDetect = 1;  //开始下一次的检测

         if(1 == ButtonStatusArray[buttonNO].short_press_counts)  //如果之前是一次单机
         {
             ButtonStatusArray[buttonNO].short_press_interval ++ ;//时间累积
             if(ButtonStatusArray[buttonNO].short_press_interval >ButtonArray_double_interval[buttonNO])  //如果超过窗口期 当做单纯的单击
             {
                ButtonStatusArray[buttonNO].short_press_counts = 0;
                ButtonStatusArray[buttonNO].short_press_interval = 0;
                ButtonStatusArray[buttonNO].short_release_callback();          
             }
         }
      }
   }
 //  ButtonStatusArray[buttonNO].StartEventDetect = 1;  //开始下一次的检测
   ButtonStatusArray[buttonNO].pinLastStatus = ButtonStatusArray[buttonNO].pinStatus ;
   return NO_BUTTON_EVENT;
}

