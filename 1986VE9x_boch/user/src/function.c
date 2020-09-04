#include "function.h"
#include "UART.h"
#include "time_scale.h"

uint8_t indication_status = 0;
int16_t dac_value         = 0x2600;
BOOL    phase_flag        = FALSE;

_Int32Union	ShiftValueTest;

/*******************************************************************
                            Запись в ПЛИС
*******************************************************************/
void Write_In_CPLD(uint8_t adr, uint8_t data){    
    MDR_PORTA->OE   = 0x0000ffff;                          // Data (0:7) - Out, Adr (8:15) - Out
    MDR_PORTA->RXTX = (uint32_t)((adr << 8) | data);
    MDR_PORTC->RXTX = (1 << 0);                            // Разрешение записи данных
    nop20();
    MDR_PORTC->RXTX = (0 << 0);                            // Запрет записи данных
    MDR_PORTA->RXTX = 0x00000000;                          // Сброс шины
}

/*******************************************************************
                         Чтение байта из ПЛИС
*******************************************************************/
uint8_t Read_From_CPLD(uint8_t adr){    
    uint8_t data;                      
	
	MDR_PORTA->OE   = 0x0000ff00;                          // Data (0:7) - Input, Adr (8:15) - Out
    MDR_PORTA->RXTX = (uint32_t)(adr << 8);
    MDR_PORTC->RXTX = (1 << 1);                            // Разрешение чтения данных
    nop20();
    data = (uint8_t)MDR_PORTA->RXTX;                       
    MDR_PORTC->RXTX = (0 << 1);                            // Запрет чтения данных
    MDR_PORTA->RXTX = 0x00000000;                          // Сброс шины
    
	return data;
}

/*******************************************************************
                                CRC
*******************************************************************/
uint16_t Calc_CRC(_MpvMsgUnion Union){
    uint8_t index;
    uint32_t crc = 0;

    // crc заголовка, длины, ID (3 слова)
    for(index = 0; index < 3; index++){
        crc += Union.Word[index];
        if(crc & 0x00010000){ // Учет переноса из 15-го бита
            crc++; 
		    crc &= 0x0000ffff;
		}
	}
    
    // crc информационной части СфдсСКС	(если есть)
    for(index = 0; index < Union.Msg.Length; index++){
        crc += Union.Msg.Data[index];
        if (crc & 0x00010000){ // Учет переноса из 15-го бита 
            crc &= 0x0000ffff;	
            crc++;
		}
	}
    return (uint16_t)crc;
}

/*******************************************************************
             Конвертация сообщений для передачи в МПВ
*******************************************************************/
void TxMsg_Convert(void){
    uint16_t length, index, tmp;
    
    length = MpvMsgUnion.Msg.Length;
    length = length * 2 + 8; // CRC, заголовок, длина и идентификатор в CRC не участвуют (+4 слова)
    for(index = 0; index < length; index += 2){
        tmp = MpvMsgUnion.Byte[index + 1];
        MpvMsgUnion.Byte[index + 1] = MpvMsgUnion.Byte[index];
        MpvMsgUnion.Byte[index] = tmp;
    }
}

/*******************************************************************
                        Управление индикацией
*******************************************************************/
void Ind_Control(uint8_t Leds, BitStatus NewState){
    if(NewState == SET){
        indication_status |= Leds;
    }else{
        indication_status &= ~Leds;
    }
    Write_In_CPLD(ADR_LEDS, indication_status);    
}

/*******************************************************************
                Установка данных в DAC(1273ПА4Т)
*******************************************************************/
void ExternalDAC_SetData(int16_t data){
    int16_t convert_data;
    uint8_t low_part, high_part;
    
    convert_data = 0x3fff - data;
    low_part     = (uint8_t)( convert_data & 0x00ff); 
    high_part    = (uint8_t)((convert_data & 0x3f00) >> 8);
    
    Write_In_CPLD(ADR_EXTDAC_LOW,  low_part);
    Write_In_CPLD(ADR_EXTDAC_HIGH, high_part);
}

/*******************************************************************
    Привидение результата измерений к виду сообщения "Измерение"
    Использовать для старого софта (008 БОЧ)
*******************************************************************/
void Mesure_Convert(int32_t data){    
    uint32_t tmp1;
    float    tmp2;
        
    if(data < 0)
        data += 0x5f5e100; 
    
    data *= 10; // Результат в нс
    tmp1  = (uint32_t)(data / 200);
    tmp2  = ((float)(data % 200) / 200) * 20.5;
    if(tmp1 != 0)   
        tmp1 += 1;
    
    BochMeasureUnion.BochMeasureMessage.x4 = (uint8_t)tmp2;    
    BochMeasureUnion.BochMeasureMessage.x5 = (uint8_t)((tmp1 & 0x007f0000) >> 16);
    BochMeasureUnion.BochMeasureMessage.x6 = (uint8_t)((tmp1 & 0x0000ff00) >> 8);
    BochMeasureUnion.BochMeasureMessage.x7 = (uint8_t)( tmp1 & 0x000000ff);
}

/*******************************************************************
                     Считывание данных из ИВИ
*******************************************************************/
int32_t Read_IVI(uint8_t adr){
	uint8_t index;
	_Int32Union	IVI; 

    IVI.Value = 0;   
    
    for(index = 0; index<=3; index++){
        IVI.Byte[index] = Read_From_CPLD(adr + index);
    }
    
    if(IVI.Value != 0x7fffffff) IVI.Value = IVI.Value; // Если на входах присутствуют оба сигнала        
    
    return IVI.Value;
}

/*******************************************************************
                Загрузка данных из сдвиговый регистр
*******************************************************************/
void Send_Shift(int32_t shift){
    uint8_t index; 

    ShiftValueTest.Value   = shift / 10; //20 // ед. мл. раз.
    
    for(index = 0; index <= 3; index++)
        Write_In_CPLD(ADR_SLOAD_DATA_1 + index, ShiftValueTest.Byte[index]);
    
    if(phase_flag == TRUE) 
        Write_In_CPLD(ADR_SLOAD, 0);
}

/*******************************************************************
                Передача сообщений (МК -> ПК)
*******************************************************************/
void Send_Msg(MsgToPC MsgType){
    uint8_t index;
    
    if(MsgType == BochStateMsg){
        for(index = 0; index < sizeof(_BochStateMessage); index++){
            UART1_sendbyte(BochStateUnion.Byte[index]);
            MDR_UART1->ICR |= (1<<5);
        }
    }else if(MsgType == BochMeasureMsg){
        for(index = 0; index < sizeof(_BochMeasureMessage); index++){
            UART1_sendbyte(BochMeasureUnion.Byte[index]);
            MDR_UART1->ICR |= (1<<5);
        }
        for(index = 0; index < 21; index++){    // баг внешнего софта
            UART1_sendbyte(0x00);
            MDR_UART1->ICR |= (1<<5);
        }
    }
}

/*******************************************************************
       Начальная инициализация структур сообщений (МК -> ПК)
*******************************************************************/
void Union_Init(void){
    Clean_Time_Scale(&BochScaleStruct);
    BochStateUnion.BohcStateMsg.Hdr1 = 0xaa;
    BochStateUnion.BohcStateMsg.Hdr2 = 0x50; // было 0x55 (исправили в протоколе)
    BochStateUnion.BohcStateMsg.Hdr3 = 0x01;
    BochMeasureUnion.BochMeasureMessage.Hdr1 = 0xaa;
    BochMeasureUnion.BochMeasureMessage.Hdr2 = 0x50;
    BochMeasureUnion.BochMeasureMessage.Hdr3 = 0x02;
}
