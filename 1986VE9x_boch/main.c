#include "main.h"

int main(void){    
    Clk_64MHz_Init();
    SysTick_Single_Shot(2);                             // �������� ��������� �����
    PortA_Init();
    PortC_Init();
    Timer1_Init();
    
    Union_Init();                                       // ��������� ������������� �������� ��������� (�� -> ��) 
    
    Write_In_CPLD(ADR_MPV_LATCH, MPV_LATCH_RESET);      // ������ ������ 1�� ���
    ExternalDAC_SetData(0);                             // ����� DAC(1273��4�) 
          
    Write_In_CPLD(ADR_UART_COMMUTATE, MPV_TO_MK);       // Uart_commutator == (MK <--> MPV)
    
    UART1_ini_PB5_PB6_std(BoudRate_115200);             // ����� ���. �����       
    UART2_ini_PD0_PD1_std(BoudRate_115200);             // ����. �����
    
    SysTick_Single_Shot(10);                            // �������� ��������� ��� (10 ���)
    
    UART_Cmd(MDR_UART2, ENABLE);                        // ���. �����    
    
    MPV_Init();                                         // ��������� ������������� ���
    SysTick_Single_Shot(2);                             // �������� 2 ���
    MPV_Config();                                       // ��������� ������ ������ ���
    
    SysTick_Single_Shot(2);                             // �������� 2 ���
    Write_In_CPLD(ADR_UART_COMMUTATE, MPV_TO_PEVM);     // Uart_commutator == (PEVM <--> MPV)  
    
    Write_In_CPLD(ADR_RELAY_HIGH, KP5_V);               // ����� ��������� ���������� ����� 
    SysTick_Single_Shot(0.33); 
    Write_In_CPLD(ADR_RELAY_HIGH, KP6_V);
    
    ExternalDAC_SetData(dac_value & 0x3fff);            // goal == 0x2800
    
    SysTick_Single_Shot(60);                            // �������� ������������� �������
        
    Write_In_CPLD(ADR_MPV_LATCH, MPV_LATCH_SET);        // ���������� ������ 1�� ���   
    
    Ext_Int1_Init();                                    // ������� ���������� �� 1�� ���
    Write_In_CPLD(ADR_RESET_FSV, 0x00);                 // ��������� ������������� �������� ������

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
