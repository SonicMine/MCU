#ifndef FUNCTION_H
#define FUNCTION_H

#include "types.h"
#include "mdr32fx.h"

/*******************************************************************
                    Шина адреса и данных
*******************************************************************/
// Разрешение/Запрет обработки сигнала 1 Гц с МПВ
#define ADR_MPV_LATCH           ((uint8_t)1)
#define MPV_LATCH_SET           ((uint8_t)1)
#define MPV_LATCH_RESET         ((uint8_t)0)

// Коммутация сигналов на входы измерителей
#define ADR_USER_MUX1           ((uint8_t)4)       // wr
#define ADR_USER_MUX2           ((uint8_t)5)       // wr
#define ADR_TUNING_MUX          ((uint8_t)6)       // wr
#define ADR_PHASING_MUX         ((uint8_t)7)       // wr
#define MUX_CHN_0_MPV           ((uint8_t)0)
#define MUX_CHN_1_AA            ((uint8_t)1)
#define MUX_CHN_2_BB            ((uint8_t)2)
#define MUX_CHN_3_CC            ((uint8_t)3)

// Управление индикацией 
#define ADR_LEDS                ((uint8_t)8)       // wr
#define IND_NUM0                ((uint8_t)8)
#define IND_NUM1                ((uint8_t)2)
#define IND_NUM2                ((uint8_t)1)
#define IND_NUM3                ((uint8_t)4)
#define IND_NUM4                ((uint8_t)16)
#define IND_NUM5                ((uint8_t)64)
#define IND_NUM6                ((uint8_t)128)
#define IND_NUM7                ((uint8_t)32)
#define ALL_IND                 ((uint8_t)255)

// Управление UART коммутатором
#define ADR_UART_COMMUTATE      ((uint8_t)9)       // wr
#define MPV_TO_MK               ((uint8_t)1)
#define MPV_TO_PEVM             ((uint8_t)0)

// Установка данных на внешний ЦАП
#define ADR_EXTDAC_LOW          ((uint8_t)10)      // wr
#define ADR_EXTDAC_HIGH         ((uint8_t)11)      // wr

// Команды управления релюшками 
#define ADR_RELAY_LOW           ((uint8_t)12)      //(wr) KP1 - KP4
#define KP1_V                   ((uint8_t)1)
#define KP1_A                   ((uint8_t)2)
#define KP2_V                   ((uint8_t)4)
#define KP2_A                   ((uint8_t)8)
#define KP3_V                   ((uint8_t)16)
#define KP3_A                   ((uint8_t)32)
#define KP4_V                   ((uint8_t)64)
#define KP4_A                   ((uint8_t)128)

// Команды управления релюшками 
#define ADR_RELAY_HIGH          ((uint8_t)13)      //(wr) KP5 - KP6
#define KP5_V                   ((uint8_t)1)
#define KP5_A                   ((uint8_t)2)
#define KP6_V                   ((uint8_t)4)
#define KP6_A                   ((uint8_t)8)

// Сброс выходных герцов по метки времени
#define ADR_RESET_FSV           ((uint8_t)14)      //wr 

// Загрузка данных в сдвиговый регистр
#define ADR_SLOAD               ((uint8_t)15)      //wr

// Расхождение между выходными герцами и меткой времени
#define ADR_SLOAD_DATA_1        ((uint8_t)16)      //wr
#define ADR_SLOAD_DATA_2        ((uint8_t)17)      //wr
#define ADR_SLOAD_DATA_3        ((uint8_t)18)      //wr
#define ADR_SLOAD_DATA_4        ((uint8_t)19)      //wr

// Измеритель временных интервалов (алгоритм подстройки частоты) 
#define ADR_IVI_FREAQ           ((uint8_t)20)      // (rd) 20...23

// Измеритель временных интервалов (алгоритм подстройки фазы)
#define ADR_IVI_PHASE           ((uint8_t)24)      // (rd) 24...27

// Измеритель временных интервалов (пользовательские входы)
#define ADR_IVI_USER            ((uint8_t)28)      // (rd) 28...31

/*******************************************************************
                            Объявления
*******************************************************************/
extern uint8_t indication_status;
extern int16_t dac_value;
extern BOOL    phase_flag;

/*******************************************************************
                            Прототипы
*******************************************************************/
uint16_t Calc_CRC           (_MpvMsgUnion Union);
uint8_t  Read_From_CPLD     (uint8_t adr);
int32_t  Read_IVI           (uint8_t adr);
void     Write_In_CPLD      (uint8_t adr, uint8_t data);
void     TxMsg_Convert      (void);
void     Ind_Control        (uint8_t Leds, BitStatus NewState);
void     ExternalDAC_SetData(int16_t data);
void     Mesure_Convert     (int32_t data);
void     Send_Shift         (int32_t shift);
void     Send_Msg           (MsgToPC MsgType);
void     Union_Init         (void);

#endif
