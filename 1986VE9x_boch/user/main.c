#include "main.h"

int main(void){    
    Clk_64MHz_Init();
    SysTick_Single_Shot(2);                             // Ожидание включения блока
    PortA_Init();
    PortC_Init();
    Timer1_Init();
    
    Union_Init();                                       // Начальная инициализация структур сообщений (МК -> ПК) 
    
    Write_In_CPLD(ADR_MPV_LATCH, MPV_LATCH_RESET);      // Запрет приема 1Гц МПВ
    ExternalDAC_SetData(0);                             // Сброс DAC(1273ПА4Т) 
          
    Write_In_CPLD(ADR_UART_COMMUTATE, MPV_TO_MK);       // Uart_commutator == (MK <--> MPV)
    
    UART1_ini_PB5_PB6_std(BoudRate_115200);             // Сразу вкл. обмен       
    UART2_ini_PD0_PD1_std(BoudRate_115200);             // Откл. обмен
    
    SysTick_Single_Shot(10);                            // Ожидание включения МПВ (10 сек)
    
    UART_Cmd(MDR_UART2, ENABLE);                        // Вкл. обмен    
    
    MPV_Init();                                         // Стартовая инициализация МПВ
    SysTick_Single_Shot(2);                             // Ожидание 2 сек
    MPV_Config();                                       // Установка режима работы МПВ
    
    SysTick_Single_Shot(2);                             // Ожидание 2 сек
    Write_In_CPLD(ADR_UART_COMMUTATE, MPV_TO_PEVM);     // Uart_commutator == (PEVM <--> MPV)  
    
    Write_In_CPLD(ADR_RELAY_HIGH, KP5_V);               // Выбор источника управления ГУНом 
    SysTick_Single_Shot(0.33); 
    Write_In_CPLD(ADR_RELAY_HIGH, KP6_V);
    
    ExternalDAC_SetData(dac_value & 0x3fff);            // goal == 0x2800
    
    SysTick_Single_Shot(60);                            // Ожидание навигационных решений
        
    Write_In_CPLD(ADR_MPV_LATCH, MPV_LATCH_SET);        // Разрешение приема 1Гц МПВ   
    
    Ext_Int1_Init();                                    // Внешнее прерывание по 1Гц МПВ
    Write_In_CPLD(ADR_RESET_FSV, 0x00);                 // Стартовая синхронизация выходных герцев

    while(1){
        if(uart2.ipRx != uart2.upRx){
            uart2.upRx = (uart2.upRx + 1) & RXRING;
            UART_ProcessMessage(uart2.RxBuf, uart2.upRx);
        }
        if(uart1.ipRx != uart1.upRx){
            uart1.upRx = (uart1.upRx + 1) & RXRING;
            UART_ProcessMessage(uart1.RxBuf, uart1.upRx);
        }  
    }
}
