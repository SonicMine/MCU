#ifndef TIMER_H
#define TIMER_H

#include "mdr32fx.h"
#include "types.h"
#include "MDR32F9Qx_it.h"

#define TIMER1_ENABLE    (MDR_TIMER1->CNTRL |= 1)
#define TIMER1_DISABLE   (MDR_TIMER1->CNTRL &= ~1)

/*******************************************************************
                            Объявления
*******************************************************************/
extern BOOL dff_flag;
extern uint8_t dff_norma_cnt;
extern uint8_t upkns_cnt;

/*******************************************************************
                            Прототипы
*******************************************************************/
void Timer1_Init(void);
void SysTick_On(float value);
void SysTick_Off(void);
void SysTick_Single_Shot(float value);

#endif
