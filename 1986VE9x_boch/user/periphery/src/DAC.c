#include "DAC.h"
#include "ports.h"

/*******************************************************************
                Инициализация модулей DAC1 и DAC2
*******************************************************************/
void DAC_init(void){
    MDR_RST_CLK->PER_CLOCK |= (1 << 25) | (1 << 18);      // Тактирование порта Е и DAC
    
    MDR_PORTE->FUNC   |=  ((2 << 0 * 2) | (2 << 9 * 2));  // режим работы порта (альт.)
    MDR_PORTE->ANALOG |=  ((0 << 0)     | (0 << 9));      // аналоговый
    MDR_PORTE->PWR    |=  ((3 << 0 * 2) | (3 << 9 * 2));  // максимально быcтрый
    MDR_PORTE->PULL   &= ~((1 << 0)     | (1 << 9) |      // pull down off
                           (1 << 16)    | (1 << 25));     // pull up off
    
    MDR_DAC->CFG      |=  ((0 << 0) |                     // опора DAC1 от AUcc
                           (0 << 1) |                     // опора DAC2 от AUcc
                           //(1 << 2) |                     // DAC1 вкл.
                           //(1 << 3) |                     // DAC2 вкл.
                           (0 << 4));                     // Синхронизация DAC1 и DAC2 - асинхронные
}

/*******************************************************************
                Управление модулями DAC1 и DAC2
*******************************************************************/
void DAC_Cmd(t_DAC_chan DAC, FunctionalState NewState){
    uint32_t tmp;

    tmp = MDR_DAC->CFG;    
    if(DAC == DAC1){
        if (NewState != DISABLE){
            tmp |=  (1 << 2);       // DAC1 вкл.
        }else{
            tmp &= ~(1 << 2);       // DAC1 выкл.
        }
    }else if(DAC == DAC2){
        if (NewState != DISABLE){
            tmp |=  (1 << 3);       // DAC2 вкл.
        }else{
            tmp &= ~(1 << 3);       // DAC2 выкл.
        }   
    }
    MDR_DAC->CFG = tmp;
}

/*******************************************************************
                Установка данных в DAC1 и DAC2
*******************************************************************/
void DAC_SetData(t_DAC_chan DAC, uint32_t Data){
    if(DAC == DAC1){
        MDR_DAC->DAC1_DATA = (Data & 0x00000fff);
    }else if(DAC == DAC2){
        MDR_DAC->DAC2_DATA = (Data & 0x00000fff);
    } 
}
