#include "timer.h"
#include "UART.h"
#include "MPV.h"
#include "function.h"
#include "PID.h"
#include "time_scale.h"
#include "algoritm.h"

int32_t  tmp_32        = 0;
uint16_t cnt_tim_1     = 0;
uint8_t  timer1_tmp    = 3;
uint8_t  dff_err_cnt   = 0;
uint8_t  upkns_cnt     = 0;
BOOL     dff_flag      = TRUE;

_Int32Union IntervalmetrChangeFreq;
_Int32Union IntervalmetrChangePhase;
_Int32Union IntervalmetrUserInputs;
_Int32Union Shift;

/*******************************************************************
                     Инициализация модуля TIMER1
*******************************************************************/
void Timer1_Init(void){  
    MDR_RST_CLK->PER_CLOCK |= 1 << 14;        // разрешение тактирования Таймера 1
    MDR_RST_CLK->TIM_CLOCK  = 0x07000000;     // TIM 1..3 ENABLE    
	
    MDR_TIMER1->CNTRL = 0;                    // таймер выкл.
	MDR_TIMER1->CNT   = 0;                    
    MDR_TIMER1->PSG   = 0;
    MDR_TIMER1->ARR   = (64000000/1000) - 1;  // 1 мс
    MDR_TIMER1->IE    = 0x00000002;           // разрешение прерывания по совпадению
    MDR_TIMER1->CNTRL = 0x00000000;           // счет вверх по TIM_CLK, таймер выкл.
	
    NVIC_EnableIRQ(TIMER1_IRQn);
}

/*******************************************************************
                     Включение модуля SYSTICK
*******************************************************************/
void SysTick_On(float value){
    SysTick->LOAD = (uint32_t)(0.77 * ((40000 * value) - 1)); //стартовое значение загружаемое в регистр VAL
    SysTick->VAL  = 0x00;
    SysTick->CTRL = ((1 << 0) |     // включение таймера
                     (1 << 1) |     // разрешение прерывания
                     (0 << 2));     // источник синхросигнала = LSI
}

/*******************************************************************
                    Выключение модуля SYSTICK
*******************************************************************/
void SysTick_Off(void){
    SysTick->CTRL &= ~(1 << 0);     //выключение таймера
    systick_it_flag = FALSE;
}

/*******************************************************************
             Заряжает SYSTICK на value секунд (Задержка) 
*******************************************************************/
void SysTick_Single_Shot(float value){
    SysTick->LOAD = (uint32_t)(0.77 * ((40000 * value) - 1)); //стартовое значение загружаемое в регистр VAL
    SysTick->VAL  = 0x00;

    SysTick->CTRL = ((1 << 0) |     // включение таймера
                     (1 << 1) |     // разрешение прерывания
                     (0 << 2));     // источник синхросигнала = LSI
    
    while(!systick_it_flag);        // Ожидание
    
    SysTick->CTRL  &= ~(1 << 0);    // выключение таймера
    systick_it_flag = FALSE;
}

/*******************************************************************
                       Прерывание от TIMER1 
*******************************************************************/
void Timer1_IRQHandler(void){
    cnt_tim_1++;
    MDR_TIMER1->CNT = 0;
    MDR_TIMER1->STATUS &= ~(1 << 1);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    if(cnt_tim_1 == 600){
        cnt_tim_1 = 0;
        IntervalmetrChangeFreq.Value  = Read_IVI(20);
        IntervalmetrChangePhase.Value = Read_IVI(24);
        IntervalmetrUserInputs.Value  = Read_IVI(28);
        
        // Передача сообщения "Измерение" // сделать передачу результата, только если выбраны каналы измерений
        Mesure_Convert(IntervalmetrUserInputs.Value);
        Send_Msg(BochMeasureMsg);        
        
        // Подстройка частоты
        ivi_dff = (double)(IntervalmetrChangeFreq.Value) - ((double)Msg151.TimeAdjust)/10;
        if(dff_flag){
            Calc_DFF();
            Tune_Freq();            
        } 

        if(fabs(dff) <= 1e-11){
            dff_err_cnt = 0;
            Ind_Control(IND_NUM0, SET);                     // вставить соотв. индикатор
            BochStateUnion.BohcStateMsg.State &= ~0x01;     // Очищаем State
            BochStateUnion.BohcStateMsg.State |= STATE_NORMA;
        }else{
            dff_err_cnt++;
            if(dff_err_cnt == 120){
                dff_err_cnt = 0;
                Ind_Control(IND_NUM0, RESET);               // вставить соотв. индикатор 
                BochStateUnion.BohcStateMsg.State &= ~0x01; // Очищаем State
                BochStateUnion.BohcStateMsg.State |= STATE_NE_NORMA;                
            }
        }
        
        if(mpv_ready & MPV_BUSY){
            upkns_cnt++;
            if(upkns_cnt == 5){
                upkns_cnt = 0;
                Ind_Control(IND_NUM0, RESET);               // вставить соотв. индикатор
            }
        }else{
            Ind_Control(IND_NUM0, SET);                     // вставить соотв. индикатор
        }
        
        // Подстройка фазы
        ivi_dt = (double)(IntervalmetrChangePhase.Value) - ((double)Msg151.TimeAdjust) / 10;
        Tune_Phase();
        
        // Передача сообщений "Состояние"
        Inc_Time_Scale(&BochScaleStruct);
        BochStateUnion.BohcStateMsg.Seconds = BochScaleStruct.Seconds;
        BochStateUnion.BohcStateMsg.Minutes = BochScaleStruct.Minutes;
        BochStateUnion.BohcStateMsg.Hours   = BochScaleStruct.Hours;
        BochStateUnion.BohcStateMsg.Days    = BochScaleStruct.Days;
        BochStateUnion.BohcStateMsg.Months  = BochScaleStruct.Months;
        BochStateUnion.BohcStateMsg.Years   = BochScaleStruct.Years;
        Send_Msg(BochStateMsg);      
        
        MDR_TIMER1->CNTRL = 0;
        NVIC_EnableIRQ(EXT_INT1_IRQn);
        NVIC_ClearPendingIRQ(EXT_INT1_IRQn);
    }
}

/*******************************************************************
                       Прерывание от SYSTICK 
*******************************************************************/
void SysTick_Handler(void){
    systick_it_flag = TRUE;
}
