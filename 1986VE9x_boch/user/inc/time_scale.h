#ifndef _TIME_SCALE
#define _TIME_SCALE

#include "mdr32fx.h"
#include "types.h"
#include <string.h>

__packed typedef struct{
    uint8_t Seconds;
    uint8_t Minutes;
    uint8_t Hours;
    uint8_t Days;
    uint8_t Months;
    uint8_t Years;
}_Time_scale;

/*******************************************************************
                            Объявления
*******************************************************************/
extern _Time_scale BochScaleStruct;

/*******************************************************************
                            Прототипы
*******************************************************************/
void Clean_Time_Scale(_Time_scale* time_scale_struct);
void Inc_Time_Scale  (_Time_scale* time_scale_struct);

#endif
