#include "MPV.h"
#include "function.h"
#include "UART.h"

_MpvMsgUnion MpvMsgUnion;
_FloatUnion  FloatUnion;

BOOL systick_it_flag = FALSE;

void MPV_Init(void){
    uint8_t  index;
    uint16_t tmp;
    
/*******************************************************************
                          Сообщение 74
*******************************************************************/    
    MpvMsgUnion.Msg.Hdr     = 0xf517;     
    MpvMsgUnion.Msg.ID      = 0x004a;
    MpvMsgUnion.Msg.Length  = 0x0001;
    MpvMsgUnion.Msg.Data[0] = 0x0001; /* Навигация на стоянке,
                                         Автоматическое переключение режимов работы - Запрещено,
                                         Запрос на выдачу вводимых данных - Не выдавать,
                                         Управление записью в ЭЗП - Не записывать   
                                      */
    MpvMsgUnion.Msg.CRC = Calc_CRC(MpvMsgUnion);
    
    //TxMsg_Convert();
    
    for(index = 0; index < 10; index++){    
        UART2_sendbyte(MpvMsgUnion.Byte[index]);
        MDR_UART2->ICR |= (1<<5);
    }
    
/*******************************************************************
                          Сообщение 07
*******************************************************************/
    MpvMsgUnion.Msg.Hdr     = 0xf517;     
    MpvMsgUnion.Msg.ID      = 0x0007;
    MpvMsgUnion.Msg.Length  = 0x0003;
    MpvMsgUnion.Msg.Data[0] = 0x0001; // Готовность значения КНФ - Готово
    MpvMsgUnion.Msg.Data[1] = 0x01ff; // 24-0 : Значение КНФ
    MpvMsgUnion.Msg.Data[2] = 0xffff; // 31-25: Резерв
    
    MpvMsgUnion.Msg.CRC = Calc_CRC(MpvMsgUnion);
    
    tmp = MpvMsgUnion.Msg.Data[1]; 
    MpvMsgUnion.Msg.Data[1] = MpvMsgUnion.Msg.Data[2];
    MpvMsgUnion.Msg.Data[2] = tmp;
        
    //TxMsg_Convert();
    
    for(index = 0; index < 14; index++){    
        UART2_sendbyte(MpvMsgUnion.Byte[index]);
        MDR_UART2->ICR |= (1<<5);
    }
}

void MPV_Config(void){
    uint8_t  index;
    uint16_t tmp;
    
/*******************************************************************
                          Сообщение 60-1
*******************************************************************/
    MpvMsgUnion.Msg.Hdr     = 0xf517;     
    MpvMsgUnion.Msg.ID      = 0x043c;
    MpvMsgUnion.Msg.Length  = 0x0008;
    MpvMsgUnion.Msg.Data[0] = (USE_PRESET_CONFIG | ALL_DATA_READY);
    
    // Использование СРНС и ДД
    MpvMsgUnion.Msg.Data[1] = (GPS_ENABLED | GLONASS_ENABLED);
    
    // Cпецификация ГНСС по диапазонам и сигналам
    MpvMsgUnion.Msg.Data[2] = 0x0000;
    MpvMsgUnion.Msg.Data[3] = (GPS_L1_EN | GLO_F1_EN |/* VT_CODE |*/ GLO_F2_EN); // if (VT_CODE != commented) { //520 признак в сообщении 107 }
    
    // Конфигурация КСВ-решения
    MpvMsgUnion.Msg.Data[4] = (TIME_SHIFT_EN | AUTO_CONTROL);
    
    // Модели динамики и число используемых ПИК
    MpvMsgUnion.Msg.Data[5] = 0x0000;
    
    // Выходные системы времени и координат
    MpvMsgUnion.Msg.Data[6] = PZ_90;
    MpvMsgUnion.Msg.Data[7] = (UTC_RUS | TC_CALENDAR | TS_ADJUSTABLE | TS_SPEED_UNLIMIT | CS_RECT);
    
    MpvMsgUnion.Msg.CRC = Calc_CRC(MpvMsgUnion);
    
    // Перевернуть слова
    tmp = MpvMsgUnion.Msg.Data[3]; 
    MpvMsgUnion.Msg.Data[3] = MpvMsgUnion.Msg.Data[2];
    MpvMsgUnion.Msg.Data[2] = tmp;

    tmp = MpvMsgUnion.Msg.Data[7]; 
    MpvMsgUnion.Msg.Data[7] = MpvMsgUnion.Msg.Data[6];
    MpvMsgUnion.Msg.Data[6] = tmp;
    
    //TxMsg_Convert();

    for(index = 0; index < 24; index++){    
        UART2_sendbyte(MpvMsgUnion.Byte[index]);
        MDR_UART2->ICR |= (1<<5);
    }
    
/*******************************************************************
                          Сообщение 63-1
*******************************************************************/
    MpvMsgUnion.Msg.Hdr    = 0xf517;
    MpvMsgUnion.Msg.ID     = 0x043f;
    MpvMsgUnion.Msg.Length = 0x0005;
    
    // Cлово управления
    MpvMsgUnion.Msg.Data[0] = (MSG151_1GZ | LINE_NUM_1);
    
    // Слово "1"    
    MpvMsgUnion.Msg.Data[1] = 0x0000;      //high
    MpvMsgUnion.Msg.Data[2] = MSG157_1GZ;  //low
    
    tmp = MpvMsgUnion.Msg.Data[2]; 
    MpvMsgUnion.Msg.Data[2] = MpvMsgUnion.Msg.Data[1];
    MpvMsgUnion.Msg.Data[1] = tmp;
    
    // Слово "2"
    MpvMsgUnion.Msg.Data[3] = 0x0000;      //high
    MpvMsgUnion.Msg.Data[4] = 0x0000;      //low
    
    tmp = MpvMsgUnion.Msg.Data[4]; 
    MpvMsgUnion.Msg.Data[4] = MpvMsgUnion.Msg.Data[3];
    MpvMsgUnion.Msg.Data[3] = tmp;
    
    MpvMsgUnion.Msg.CRC = Calc_CRC(MpvMsgUnion);
    
    //TxMsg_Convert();

    for(index = 0; index < 18; index++){    
        UART2_sendbyte(MpvMsgUnion.Byte[index]);
        MDR_UART2->ICR |= (1<<5);
    }
    
/*******************************************************************
                          Сообщение 64-0
*******************************************************************/
    MpvMsgUnion.Msg.Hdr      = 0xf517;
    MpvMsgUnion.Msg.ID       = 0x0040;
    MpvMsgUnion.Msg.Length   = 0x0013;
    
    // Cлово управления
    MpvMsgUnion.Msg.Data[0]  = (SINGLE_DATA_TRANSFER | ACCURACY_READY | USE_PRESET_WORKMODE); 
    
    // Параметры
    MpvMsgUnion.Msg.Data[1]  = 0x0000; // high FP
    MpvMsgUnion.Msg.Data[2]  = 0x0000; // low  FP
    MpvMsgUnion.Msg.Data[3]  = 0x0000; // high FP
    MpvMsgUnion.Msg.Data[4]  = 0x0000; // low  FP
    MpvMsgUnion.Msg.Data[5]  = 0x0000; // high FP
    MpvMsgUnion.Msg.Data[6]  = 0x0000; // low  FP
    MpvMsgUnion.Msg.Data[7]  = 0x0000; // high FP
    MpvMsgUnion.Msg.Data[8]  = 0x0000; // low  FP
    MpvMsgUnion.Msg.Data[9]  = 0x0000; // high FP
    MpvMsgUnion.Msg.Data[10] = 0x0000; // low  FP
    MpvMsgUnion.Msg.Data[11] = 0x0000; // high FP
    MpvMsgUnion.Msg.Data[12] = 0x0000; // low  FP
    MpvMsgUnion.Msg.Data[13] = 0x0000; // high FP
    MpvMsgUnion.Msg.Data[14] = 0x0000; // low  FP
    MpvMsgUnion.Msg.Data[15] = 0x0000; // high FP
    MpvMsgUnion.Msg.Data[16] = 0x0000; // low  FP
    
    //Data не конвертирована, так как Параметры == 0;
    
    FloatUnion.Value         = 10;
    MpvMsgUnion.Msg.Data[17] = FloatUnion.Word[0]; // lowFP
    MpvMsgUnion.Msg.Data[18] = FloatUnion.Word[1]; // high FP
    
    MpvMsgUnion.Msg.CRC = Calc_CRC(MpvMsgUnion);
    
    //TxMsg_Convert();
    
    for(index = 0; index < 46; index++){    
        UART2_sendbyte(MpvMsgUnion.Byte[index]);
        MDR_UART2->ICR |= (1<<5);
    }
    
/*******************************************************************
                          Сообщение 84-0
*******************************************************************/
    MpvMsgUnion.Msg.Hdr     = 0xf517;
    MpvMsgUnion.Msg.ID      = 0x0054;
    MpvMsgUnion.Msg.Length  = 0x0001; 
     
    // Cлово управления
    MpvMsgUnion.Msg.Data[0] = ALWAYS_1HZ_MPV;
    
    MpvMsgUnion.Msg.CRC     = Calc_CRC(MpvMsgUnion);
    
    for(index = 0; index < 10; index++){    
        UART2_sendbyte(MpvMsgUnion.Byte[index]);
        MDR_UART2->ICR |= (1<<5);
    }
}
