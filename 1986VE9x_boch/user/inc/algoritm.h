#ifndef ALGORITM_H
#define ALGORITM_H

#include "types.h"
#include "mdr32fx.h"

/*******************************************************************
                            Объявления
*******************************************************************/
extern uint8_t restart_dff;
extern double  ivi_dff; 
extern double  ivi_dt;
extern double  count_i;
extern double  sum1;
extern double  sum2;
extern double  dff;

/*******************************************************************
                            Прототипы
*******************************************************************/
void Calc_DFF   (void);
void Tune_Freq  (void);
void Tune_Phase (void);

#endif
