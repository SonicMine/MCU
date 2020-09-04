#ifndef TYPES_H
#define TYPES_H

#include "mdr32fx.h"
#include "MPV_messages.h"
#include "PEVM_messages.h"
#include "math.h"

#define     nop5()  {__NOP(); __NOP(); __NOP(); __NOP(); __NOP();}
#define     nop20() {nop5(); nop5(); nop5(); nop5();}

typedef enum {FALSE = 0, TRUE = !FALSE} BOOL;

/*******************************************************************
                        Упаковка float
*******************************************************************/
typedef union{
    float    Value;
    uint16_t Word[2];
}_FloatUnion;

/*******************************************************************
                        Упаковка int32
*******************************************************************/
typedef union{
    int32_t Value;
    uint8_t Byte[4];
}_Int32Union;

/*******************************************************************
    Информационная часть сообщения "Установка времени" (ПК -> МК)
*******************************************************************/
__packed typedef struct{
    uint8_t Seconds;
    uint8_t Minutes;
    uint8_t Hours;
    uint8_t Days;
    uint8_t Months;
    uint8_t Years;
}_SetTimeMessage;

/*******************************************************************
          Информационная часть сообщения 151-1 (МПВ -> МК)
*******************************************************************/
__packed typedef struct{
    uint16_t SolutionFlags;     // Оперативные признаки навигационно-временного решения
    uint16_t OutputSystems;     // Выходные системы координат и времени
    uint16_t TimeSource;        // Основные источники определе-ния времени
    uint8_t  Years;             //
    uint8_t  Month;             // Календарная дата 
    uint8_t  Days;              //
    uint8_t  Hours;             //
    uint8_t  Minuts;            // Календарное время 
    uint8_t  Seconds;           //
    int32_t  TimeAdjust;        // Поправка времени (-+500 000 000 нс)
    double   CoordX;
    double   CoordY;
    double   CoordZ;
}_Message151;

/*******************************************************************
                Сообщение "Состояние БОЧ" (МК -> ПК)
*******************************************************************/
__packed typedef struct{
    uint8_t Hdr1;
    uint8_t Hdr2;
    uint8_t Hdr3;
    uint8_t State;              // 0 бит - норма/не норма; 1-2 биты - шкала(00 - КНС, 11 - свободный ход, 10 - шкала А, 01 - шкала В)
    uint8_t Seconds;
    uint8_t Minutes;
    uint8_t Hours;
    uint8_t Days;
    uint8_t Months;
    uint8_t Years;
}_BochStateMessage;

typedef union{
    _BochStateMessage BohcStateMsg;
    uint8_t Byte[sizeof(_BochStateMessage)]; 
}_BochStateUnion;

/*******************************************************************
                  Сообщение "Измерение" (МК -> ПК)
*******************************************************************/
__packed typedef struct{
    uint8_t Hdr1;
    uint8_t Hdr2;
    uint8_t Hdr3;
    uint8_t What;
    uint8_t x4;
    uint8_t x5;
    uint8_t x6;
    uint8_t x7;
}_BochMeasureMessage;

typedef union{
    _BochMeasureMessage BochMeasureMessage;
    uint8_t Byte[sizeof(_BochMeasureMessage)]; 
}_BochMeasureUnion;

/*******************************************************************
    Общая структура для формирования сообщений МПВ (МК -> МПВ)
*******************************************************************/
__packed typedef struct{
    uint16_t Hdr;	
    uint16_t ID;
    uint16_t Length;
    uint16_t CRC;
    uint16_t Data[50];	
}_MpvMessage;

typedef union{
    _MpvMessage Msg;
    uint8_t  Byte[100];
    uint16_t Word[50];
}_MpvMsgUnion;

/*******************************************************************
                  Сообщение "Debug algoritm" (МК -> ПК)
*******************************************************************/
__packed typedef struct{
    uint8_t Hdr1;
    uint8_t Hdr2;
    double  Ivi_Dff;
    double  Dff;
    double  Count_i;
}_DebugAlgoritmMessage;

typedef union{
    _DebugAlgoritmMessage DebugAlgoritmMessage;
    uint8_t Byte[sizeof(_DebugAlgoritmMessage)]; 
}_DebugAlgoritmUnion;

/*******************************************************************
                            Объявления
*******************************************************************/
extern volatile _SetTimeMessage     SetTimeMsg;
extern          _MpvMsgUnion        MpvMsgUnion;
extern          _FloatUnion         FloatUnion;
extern          _BochStateUnion     BochStateUnion; 
extern          _BochMeasureUnion   BochMeasureUnion;
extern          _Message151         Msg151;
extern          _Int32Union         IntervalmetrChangeFreq;
extern          _Int32Union         IntervalmetrChangePhase;
extern          _Int32Union         IntervalmetrUserInputs;
extern          _Int32Union         Shift;

#endif
