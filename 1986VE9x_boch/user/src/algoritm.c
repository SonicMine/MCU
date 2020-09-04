#include "algoritm.h"
#include "function.h"

uint8_t restart_dff = 0;    // Признак сброса значения ДЗЧ
double  ivi_dff     = 0.0;  // Расхождение импульсов (для подстройки частоты)
double  ivi_dt      = 0.0;  // Расхождение импульсов (для подстройки фазы) 
double  count_i     = 0.0;  // Номер измерения
double  sum1        = 0.0;  // Составляющая для расчета ДЗЧ
double  sum2        = 0.0;  // Составляющая для расчета ДЗЧ
double  dff         = 0.0;  // Значение ДЗЧ

/*******************************************************************
                            Расчет ДЗЧ 
*******************************************************************/
void Calc_DFF(void){ 
	if(restart_dff || (count_i >= 7200)){
        count_i = 0.0;							    // сбрасываем счётчик измерений
		sum1    = 0.0;							    // обнуляем сумму sum1 для расчёта dff
		sum2    = 0.0;                              // обнуляем сумму sum2 для расчёта dff
	}	
	count_i++;										// Инкрементируем счётчик измерений
    sum1 += ivi_dff * count_i;                      // Считаем sum1
	sum2 += ivi_dff;						        // Считаем sum2 
	if(count_i >= 20)                               // Считаем ДЗЧ
        dff = ((6 * (2 * sum1 - (count_i + 1) * sum2)) / (count_i * (count_i - 1) * (count_i + 1))) * 1e-9;	
}

/*******************************************************************
                    Алгоритм подстройки частоты 
*******************************************************************/
void Tune_Freq(void){
    double correction_freq;
    
    if(restart_dff){	
        restart_dff = 0;			 // если установлен флаг перезапуска алгоритма сбрасываем флаг перезапуска алгоритма
	}else{
        if(((count_i >= 22)   && (fabs(dff) >= 5e-8))  ||       // 22 сек
           ((count_i >= 60)   && (fabs(dff) >= 1e-9))  ||       // 60 сек
           ((count_i >= 180)  && (fabs(dff) >= 1e-10)) ||       // 300 сек
           ((count_i >= 300)  && (fabs(dff) >= 1e-11)) ||       // 1000 сек
           ((count_i >= 1200) && (fabs(dff) >= 9e-12)) ||
           ((count_i >= 3600) && (fabs(dff) >= 7e-12)) //||       // 3600 сек
           /*((count_i >= 7200) && (fabs(dff) >= 8e-12))*/){	    // 7200 сек  
            correction_freq = dff/3.66e-11;
               
            if(correction_freq >= 0){
                if((correction_freq - (int32_t)correction_freq) > 0.2){
                    correction_freq += 1.0;                   
                }
            }else{
                if((correction_freq - (int32_t)correction_freq) < -0.2){
                    correction_freq -= 1.0;
                }
            }
            
            dac_value -= (int16_t)correction_freq;
            ExternalDAC_SetData(dac_value & 0x3fff);   
		    restart_dff = 1;				                // Перезапускаем алгоритм
        }
	}
}

/*******************************************************************
                    Алгоритм подстройки фазы 
*******************************************************************/
void Tune_Phase(void){
    if(fabs(ivi_dt) > 200) Write_In_CPLD(ADR_RESET_FSV, 0x00);
    if(fabs(ivi_dt) > 10){ // если расхождение больше 200 нс
        Shift.Value = (-1)*ivi_dt;
    }else{
        Shift.Value = 0;
    }
    Send_Shift(Shift.Value);
}

