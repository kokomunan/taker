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
        if(ButtonArray_polar[BIndex] == GPIO_GET_PIN(ButtonArray[BIndex]))  //��ȡio
        {
            SETBIT(button_status,BIndex);  //��������� ����λ
         
        }	
        else
        {
            CLRBIT(button_status,BIndex);   //���û�а��¾����λ
        				
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
		ButtonStatusArray[i].LongPressThreshold = LONG_PRESS_TIME;  //������ʱ��
		ButtonStatusArray[i].short_press_counts = 0;
		ButtonStatusArray[i].short_press_interval = 0;
		CLRBIT(button_status,i);        	        
		ButtonStatusArray[i].pinStatus = GetButtonStatus((ButtonsNumber)i); //��¼״̬����
		ButtonStatusArray[i].pinLastStatus = ButtonStatusArray[i].pinStatus; //��¼�ϴ�����״̬
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
    ButtonStatusArray[buttonNO].pinStatus = GetButtonStatus(buttonNO); //�õ���ǰ����״̬

   if((0 == ButtonStatusArray[buttonNO].StartEventDetect)
      &&(BUTTONS_ACTIVE_STATE == ButtonStatusArray[buttonNO].pinStatus))//��������� ����û�м�⿪ʼ
     {
         ButtonStatusArray[buttonNO].pinLastStatus = ButtonStatusArray[buttonNO].pinStatus ;  //����״̬
         return NO_BUTTON_EVENT;  //���¼�
      }

   if(BUTTONS_ACTIVE_STATE == ButtonStatusArray[buttonNO].pinStatus) //�����������
   {
      if(ButtonStatusArray[buttonNO].pinStatus != ButtonStatusArray[buttonNO].pinLastStatus) //����״̬�����仯
      {                  
        ButtonStatusArray[buttonNO].buttonPressedTimeslots = 0;  //���µ�ʱ������
		
      }
      else //���֮ǰҲ�ǰ��� ����Ҳ�ǰ���
      {
         
         ButtonStatusArray[buttonNO].buttonPressedTimeslots ++; //����ʱ���ۻ�
         if(ButtonStatusArray[buttonNO].LongPressThreshold < ButtonStatusArray[buttonNO].buttonPressedTimeslots) //������µ��ۼ�ʱ�䳬��200tick
         {
  
            ButtonStatusArray[buttonNO].long_release_callback();     //�ص�����   

         }         
      }
   }
   else // ��������ͷ���
   {
      if(ButtonStatusArray[buttonNO].pinStatus != ButtonStatusArray[buttonNO].pinLastStatus) //����״̬�仯  ֮ǰ�ǰ��µ�
      {
         if((ButtonStatusArray[buttonNO].buttonPressedTimeslots < ButtonStatusArray[buttonNO].LongPressThreshold)  
         &&(ButtonStatusArray[buttonNO].buttonPressedTimeslots > SHORT_PRESS_TIME))// ����������µ�ʱ����ڶ̰���ʱ�� ��С�ڳ�����
         {
            ButtonStatusArray[buttonNO].buttonPressedTimeslots = 0;  //���µ�ʱ������
            ButtonStatusArray[buttonNO].short_press_counts ++;  //�����¼��ۼ�
            ButtonStatusArray[buttonNO].short_press_interval = 0; //�̰�ʱ������
            if(ButtonStatusArray[buttonNO].short_press_counts > 1)  //�������1 ˵���ϴη�����һ��
            {
                ButtonStatusArray[buttonNO].short_press_counts = 0; //�����ۼ�������
                ButtonStatusArray[buttonNO].double_press_callback();  //�ص�˫���¼�       
            }
           
         } 
				 
      }
      else  //����û�б仯 ֮ǰҲ���ͷŵ�
      {
         //start next button event detecting
         ButtonStatusArray[buttonNO].StartEventDetect = 1;  //��ʼ��һ�εļ��

         if(1 == ButtonStatusArray[buttonNO].short_press_counts)  //���֮ǰ��һ�ε���
         {
             ButtonStatusArray[buttonNO].short_press_interval ++ ;//ʱ���ۻ�
             if(ButtonStatusArray[buttonNO].short_press_interval >ButtonArray_double_interval[buttonNO])  //������������� ���������ĵ���
             {
                ButtonStatusArray[buttonNO].short_press_counts = 0;
                ButtonStatusArray[buttonNO].short_press_interval = 0;
                ButtonStatusArray[buttonNO].short_release_callback();          
             }
         }
      }
   }
 //  ButtonStatusArray[buttonNO].StartEventDetect = 1;  //��ʼ��һ�εļ��
   ButtonStatusArray[buttonNO].pinLastStatus = ButtonStatusArray[buttonNO].pinStatus ;
   return NO_BUTTON_EVENT;
}

