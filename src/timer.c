
//SysTick_Config(SystemCoreClock / 1000);
#include "timer.h"
//#include "roleta.h" todo
#include "stm32f0xx.h"

unsigned long timetick=0;
unsigned long timesec=0;

int timer_init(unsigned long tick_freq){
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / tick_freq);
  return 0;
}

void SysTick_Handler(void)  /// systick interrupt
{
  //TimingDelay_Decrement();
  // sample_input_states(); todo
  timetick++;
  if((timetick%1000)==0)timesec++;
}

t_timer timer_duration(t_timer * timer)
{
	return (timetick - *timer);
}

int timeout(t_timer * timer, unsigned long timeout)
{
	volatile unsigned long tmp;
	tmp=timetick;
	if(timeout==0)
	{
		*timer=timetick;
		return 1;
	}
	if((tmp-*timer)>=timeout)
	{
		*timer=timetick;
		return 1;
	}
	return 0;

}

void delay(unsigned int ms)
{
	t_timer delay_timer;
	timeout(&delay_timer,0);
	while(!timeout(&delay_timer,ms));
}



