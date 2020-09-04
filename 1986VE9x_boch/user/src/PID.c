#include "PID.h"

_PID_TypeDef pid;

/*******************************************************************
                   Инициализация структуры PID 
*******************************************************************/
void PID_Init(_PID_TypeDef* pid, float kP, float kI, float kD, float goal){
    pid->kP     = kP;
    pid->kI     = kI;
    pid->kD     = kD;
    pid->Goal   = goal;
}

/*******************************************************************
                          PID регулятор
*******************************************************************/
float PID_Regulation(_PID_TypeDef* pid, int32_t* input){
    float tmp_input,
          //Int,              // Накопитель
          pr_result,          // Пропорциональный вклад в регулирование
          int_result,        // Интегральный вклад в регулирование
          def_result,        // Дифференциальный вклад в регулирование 
          prev_error,        // Предыдущее значение ошибки
          result;           // Управляющее воздействие регулятора
    
    tmp_input = (float)(*input);
    
    prev_error =  pid->Error;
    pid->Error =  pid->Goal - tmp_input;
    pid->Int   =  pid->Int + (prev_error + pid->Error)/2;
    pr_result  =  pid->Error * pid->kP; 
    int_result =  pid->Int / pid->kI;
    def_result = (pid->Error - prev_error) * pid->kD;    
    result     =  pr_result + int_result + def_result;
    
    return result;
}
