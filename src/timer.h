
#ifndef TIMER_H_
#define TIMER_H_



//#define TTICK (timetick)
#define TTICK (TIM2->CNT)

typedef volatile unsigned long t_timer;
int timeout(t_timer * timer, unsigned long timeout);
int timer_init(unsigned long tick_freq);
void delay(unsigned int ms);
t_timer timer_duration(t_timer * timer);
extern unsigned long timetick;
extern unsigned long timesec;
#define now() timetick



#endif /* TIMER_H_ */
