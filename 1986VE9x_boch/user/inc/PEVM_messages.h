#ifndef PEVM_MSG_H
#define PEVM_MSG_H

typedef enum {BochStateMsg, BochMeasureMsg} MsgToPC;

/*******************************************************************
                     Типы команд ПК -> МК
*******************************************************************/
#define SETTIME             ((uint8_t)0x01)                    // Установка времени
#define MEASURE             ((uint8_t)0x02)                    // Измерение 
#define TUNING              ((uint8_t)0x05)                    // Подстройка
#define PHASING             ((uint8_t)0x06)                    // Фазировка герца 

/*******************************************************************
                     Команда MEASURE (ПК -> МК)
*******************************************************************/
#define TUNING_OFF          ((uint8_t)0x00)                    // Отключение подстройки
#define RESET_MK            ((uint8_t)0xff)                    // Сброс МК 
#define CHANNELS_A_B        ((uint8_t)0x01)                    // Измерение A-B
#define CHANNELS_A_F1       ((uint8_t)0x02)                    // Измерение A-F1
#define CHANNELS_A_C        ((uint8_t)0x07)                    // Измерение A-C
#define CHANNELS_B_F1       ((uint8_t)0x03)                    // Измерение B-F1
#define CHANNELS_B_A        ((uint8_t)0x04)                    // Измерение B-A
#define CHANNELS_B_C        ((uint8_t)0x08)                    // Измерение B-C
#define CHANNELS_F1_A       ((uint8_t)0x05)                    // Измерение F1-A
#define CHANNELS_F1_B       ((uint8_t)0x06)                    // Измерение F1-B
#define CHANNELS_F1_C       ((uint8_t)0x09)                    // Измерение F1-C
#define CHANNELS_C_A        ((uint8_t)0x0A)                    // Измерение C-A
#define CHANNELS_C_B        ((uint8_t)0x0B)                    // Измерение C-B
#define CHANNELS_C_F1       ((uint8_t)0x0C)                    // Измерение C-F1

/*******************************************************************
                     Команда TUNING (ПК -> МК)
*******************************************************************/
#define TUNING_KNF          ((uint8_t)0x01)                    // Подстройка КНФ
#define TUNING_A            ((uint8_t)0x21)                    // Подстройка А
#define TUNING_B            ((uint8_t)0x31)                    // Подстройка В
#define TUNING_C            ((uint8_t)0x41)                    // Подстройка C

/*******************************************************************
                     Команда PHASING (ПК -> МК)
*******************************************************************/
#define PHASING_KNF         ((uint8_t)0x01)                    // Фазировка КНФ
#define PHASING_A           ((uint8_t)0x21)                    // Фазировка А
#define PHASING_B           ((uint8_t)0x31)                    // Фазировка В
#define PHASING_C           ((uint8_t)0x41)                    // Фазировка C

/*******************************************************************
                     Команда Состояние (МК -> ПК)
*******************************************************************/
#define STATE_NORMA         ((uint8_t)0x01)
#define STATE_NE_NORMA      ((uint8_t)0x00)
#define STATE_SHV_KNS       ((uint8_t)0x00)                     // Подстройка под КНС  
#define STATE_SHV_FREE      ((uint8_t)0x06)                     // Свободный ход (без подстройки)
#define STATE_SHV_A         ((uint8_t)0x04)                     // Подстройка под вход А
#define STATE_SHV_B         ((uint8_t)0x02)                     // Подстройка под вход Б    

#endif
