#ifndef PID_H
#define PID_H

#include "mdr32fx.h"
#include "types.h"

typedef struct{
    float kP;               // Пропорциональный коэффициент
    float kI;               // Интегральный коэффициент
    float kD;               // Дифференциальный коэффициент
    float Goal;             // Цель
    float Error;            // Текущее значение ошибки
    float Int;              // Текущее значение интегратора    
} _PID_TypeDef;

/*******************************************************************
                            Объявления
*******************************************************************/
extern _PID_TypeDef pid;

/*******************************************************************
                            Прототипы
*******************************************************************/
void PID_Init(_PID_TypeDef* pid, float kP, float kI, float kD, float goal);
float PID_Regulation(_PID_TypeDef* pid, int32_t* input);

#endif
