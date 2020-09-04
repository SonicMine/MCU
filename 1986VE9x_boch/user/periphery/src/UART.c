#include "UART.h"
#include "function.h"
#include "DAC.h"
#include "timer.h"
#include "PEVM_messages.h"
#include "time_scale.h"

volatile _SetTimeMessage SetTimeMsg;
volatile _UART_TypeDef uart2, uart1;

_BochStateUnion     BochStateUnion;
_BochMeasureUnion   BochMeasureUnion;
_Message151         Msg151;

uint16_t mpv_ready = 0;

/*******************************************************************
                    Парсер/Формирователь сообщений
*******************************************************************/
void UART_ProcessMessage(volatile uint8_t* buf, uint8_t ptr){
    uint8_t msg_type, conf_byte;

    msg_type  = buf[(ptr - 1) & BUFFSIZE];     // Тип сообщения (ПК -> МК)
    conf_byte = buf[ ptr      & BUFFSIZE];     // Параметр сообщения (ПК -> МК)  
    if((buf[(ptr - 3) & BUFFSIZE] == 0xaa) && (buf[(ptr - 2) & BUFFSIZE] == 0x50)){
        switch(msg_type){
            case MEASURE:{  
                if(conf_byte == TUNING_OFF){             // Отключение подстройки 
                    //dff_flag   = FALSE;
                    phase_flag = FALSE;
                    BochStateUnion.BohcStateMsg.State &= ~0x06; // Очищаем State
                    BochStateUnion.BohcStateMsg.State |= STATE_SHV_FREE;
                }else if(conf_byte == RESET_MK){                // Сброс МК
                    __NOP();
                }else if(conf_byte == CHANNELS_A_B){            // Измерение A-B
                    Write_In_CPLD(ADR_USER_MUX1, MUX_CHN_1_AA);
                    Write_In_CPLD(ADR_USER_MUX2, MUX_CHN_2_BB);
                    BochMeasureUnion.BochMeasureMessage.What = 0x01;
                }else if(conf_byte == CHANNELS_A_C){            // Измерение A-C
                    Write_In_CPLD(ADR_USER_MUX1, MUX_CHN_1_AA);
                    Write_In_CPLD(ADR_USER_MUX2, MUX_CHN_3_CC);
                }else if(conf_byte == CHANNELS_A_F1){           // Измерение A-F1
                    Write_In_CPLD(ADR_USER_MUX1, MUX_CHN_1_AA);
                    Write_In_CPLD(ADR_USER_MUX2, MUX_CHN_0_MPV);
                    BochMeasureUnion.BochMeasureMessage.What = 0x02;
                }else if(conf_byte == CHANNELS_B_A){            // Измерение B-A
                    Write_In_CPLD(ADR_USER_MUX1, MUX_CHN_2_BB);
                    Write_In_CPLD(ADR_USER_MUX2, MUX_CHN_1_AA);
                    BochMeasureUnion.BochMeasureMessage.What = 0x04;
                }else if(conf_byte == CHANNELS_B_C){            // Измерение B-C
                    Write_In_CPLD(ADR_USER_MUX1, MUX_CHN_2_BB);
                    Write_In_CPLD(ADR_USER_MUX2, MUX_CHN_3_CC);
                }else if(conf_byte == CHANNELS_B_F1){           // Измерение B-F1
                    Write_In_CPLD(ADR_USER_MUX1, MUX_CHN_2_BB);
                    Write_In_CPLD(ADR_USER_MUX2, MUX_CHN_0_MPV);
                    BochMeasureUnion.BochMeasureMessage.What = 0x03;
                }else if(conf_byte == CHANNELS_C_A){            // Измерение C-A
                    Write_In_CPLD(ADR_USER_MUX1, MUX_CHN_3_CC);
                    Write_In_CPLD(ADR_USER_MUX2, MUX_CHN_1_AA);
                }else if(conf_byte == CHANNELS_C_B){            // Измерение C-B
                    Write_In_CPLD(ADR_USER_MUX1, MUX_CHN_3_CC);
                    Write_In_CPLD(ADR_USER_MUX2, MUX_CHN_2_BB);
                }else if(conf_byte == CHANNELS_C_F1){           // Измерение C-F1
                    Write_In_CPLD(ADR_USER_MUX1, MUX_CHN_3_CC);
                    Write_In_CPLD(ADR_USER_MUX2, MUX_CHN_0_MPV);
                }else if(conf_byte == CHANNELS_F1_A){           // Измерение F1-A
                    Write_In_CPLD(ADR_USER_MUX1, MUX_CHN_0_MPV);
                    Write_In_CPLD(ADR_USER_MUX2, MUX_CHN_1_AA);
                    BochMeasureUnion.BochMeasureMessage.What = 0x05;
                }else if(conf_byte == CHANNELS_F1_B){           // Измерение F1-B
                    Write_In_CPLD(ADR_USER_MUX1, MUX_CHN_0_MPV);
                    Write_In_CPLD(ADR_USER_MUX2, MUX_CHN_2_BB);
                    BochMeasureUnion.BochMeasureMessage.What = 0x06;
                }else if(conf_byte == CHANNELS_F1_C){           // Измерение F1-C
                    Write_In_CPLD(ADR_USER_MUX1, MUX_CHN_0_MPV);
                    Write_In_CPLD(ADR_USER_MUX2, MUX_CHN_3_CC);
                }
                break;                
            }
            case TUNING:{ 
                phase_flag = TRUE;                              // Вкл. подстройка фазы
                //dff_flag   = TRUE;                            // Вкл. подстройка частоты
                if(conf_byte == TUNING_KNF){                    // Подстройка КНС 
                    Write_In_CPLD(ADR_TUNING_MUX, MUX_CHN_0_MPV);
                    BochStateUnion.BohcStateMsg.State &= ~0x06; // Очищаем State
                    BochStateUnion.BohcStateMsg.State |= STATE_SHV_KNS;
                    Write_In_CPLD(ADR_RESET_FSV, 0x00);
                }else if(conf_byte == TUNING_A){                // Подстройка А
                    Write_In_CPLD(ADR_TUNING_MUX, MUX_CHN_1_AA);
                    BochStateUnion.BohcStateMsg.State &= ~0x06; // Очищаем State
                    BochStateUnion.BohcStateMsg.State |= STATE_SHV_A;
                    Write_In_CPLD(ADR_RESET_FSV, 0x00);
                }else if(conf_byte == TUNING_B){                // Подстройка B
                    Write_In_CPLD(ADR_TUNING_MUX, MUX_CHN_2_BB);
                    BochStateUnion.BohcStateMsg.State &= ~0x06; // Очищаем State
                    BochStateUnion.BohcStateMsg.State |= STATE_SHV_B;
                    Write_In_CPLD(ADR_RESET_FSV, 0x00);
                }else if(conf_byte == TUNING_C){                // Подстройка C
                    Write_In_CPLD(ADR_TUNING_MUX, MUX_CHN_3_CC);
                    Write_In_CPLD(ADR_RESET_FSV, 0x00);
                }
                break;
            }
            case PHASING:{
                phase_flag = FALSE;                             // Выкл. подстройка фазы                
                if(conf_byte == PHASING_KNF){                   // Фазировка герца под КНС 
                    Write_In_CPLD(ADR_PHASING_MUX, MUX_CHN_0_MPV);
                    Write_In_CPLD(ADR_RESET_FSV, 0x00);
                }else if(conf_byte == PHASING_A){               // Фазировка герца под вход А
                    Write_In_CPLD(ADR_PHASING_MUX, MUX_CHN_1_AA);
                    Write_In_CPLD(ADR_RESET_FSV, 0x00);
                }else if(conf_byte == PHASING_B){               // Фазировка герца под вход B
                    Write_In_CPLD(ADR_PHASING_MUX, MUX_CHN_2_BB);
                    Write_In_CPLD(ADR_RESET_FSV, 0x00);
                }else if(conf_byte == PHASING_C){               // Фазировка герца под вход C
                    Write_In_CPLD(ADR_PHASING_MUX, MUX_CHN_3_CC);
                    Write_In_CPLD(ADR_RESET_FSV, 0x00);
                }
                break;
            }
            default:
                break;
        }
    }
    // Сообщение на установку времени (ПК -> МК)
    if((buf[(ptr - 9) & BUFFSIZE] == 0xaa) && (buf[(ptr - 8) & BUFFSIZE] == 0x50) && (buf[(ptr - 7) & BUFFSIZE] == SETTIME) && (buf[ptr & BUFFSIZE] == 0x00)){
        BochScaleStruct.Seconds = buf[(ptr - 6) & BUFFSIZE];
        BochScaleStruct.Minutes = buf[(ptr - 5) & BUFFSIZE];
        BochScaleStruct.Hours   = buf[(ptr - 4) & BUFFSIZE];
        BochScaleStruct.Days    = buf[(ptr - 3) & BUFFSIZE];
        BochScaleStruct.Months  = buf[(ptr - 2) & BUFFSIZE];
        BochScaleStruct.Years   = buf[(ptr - 1) & BUFFSIZE];
    }
    // Прием сообщения 107-1 (информация о событии) (МПВ -> МК)
    if((buf[(ptr - 13) & BUFFSIZE] == 0x17) && (buf[(ptr - 12) & BUFFSIZE] == 0xf5) && (buf[(ptr - 11) & BUFFSIZE] == 0x6b) && (buf[(ptr - 10) & BUFFSIZE] == 0x04)){
        if((buf[(ptr - 5) & BUFFSIZE] == 0x4d) && (buf[(ptr - 4) & BUFFSIZE] == 0x01)){ // 333 - выходной сигнал "1 Гц" отстоит от указанной ШВ более чем на 500 нс
            __NOP();
        }else if((buf[(ptr - 5) & BUFFSIZE] == 0x08) && (buf[(ptr - 4) & BUFFSIZE] == 0x02)){ // 520 - не введен КНФ
            __NOP();
        }
    }
    // Прием сообщения 101-1 (результаты самоконтроля) (МПВ -> МК)
    if((buf[(ptr - 19) & BUFFSIZE] == 0x17) && (buf[(ptr - 18) & BUFFSIZE] == 0xf5) && (buf[(ptr - 17) & BUFFSIZE] == 0x65) && (buf[(ptr - 16) & BUFFSIZE] == 0x04)){
        __NOP();    
    }
    // Прием сообщения 151-1 (решение: координаты, скорость, время) (МПВ -> МК)
    if((buf[(ptr - 97) & BUFFSIZE] == 0x17) && (buf[(ptr - 96) & BUFFSIZE] == 0xf5) && (buf[(ptr - 95) & BUFFSIZE] == 0x97) && (buf[(ptr - 94) & BUFFSIZE] == 0x04)){
        Msg151.Years        =  buf[(ptr - 83) & BUFFSIZE];            // 15 - 18 // -83....-80
        Msg151.Month        =  buf[(ptr - 82) & BUFFSIZE];  
        Msg151.Days         =  buf[(ptr - 81) & BUFFSIZE];
        Msg151.Hours        =  buf[(ptr - 79) & BUFFSIZE];            // 19 - 22 // -79....-76
        Msg151.Minuts       =  buf[(ptr - 78) & BUFFSIZE];
        Msg151.Seconds      =  buf[(ptr - 77) & BUFFSIZE];
        Msg151.TimeAdjust   = (int32_t)
                              (buf[(ptr - 75) & BUFFSIZE]        | 
                              (buf[(ptr - 74) & BUFFSIZE] << 8)  |
                              (buf[(ptr - 73) & BUFFSIZE] << 16) | 
                              (buf[(ptr - 72) & BUFFSIZE] << 24));    // 23 - 26 // -75....-72     
    
        mpv_ready        = (buf[(ptr - 89) & BUFFSIZE]        | 
                              (buf[(ptr - 88) & BUFFSIZE] << 8)) ;
    }
    // Прием сообщения 157-1 (состояние каналов) (МПВ -> МК) ???????
    if((buf[(ptr - 97) & BUFFSIZE] == 0x17) && (buf[(ptr - 96) & BUFFSIZE] == 0xf5) && (buf[(ptr - 95) & BUFFSIZE] == 0x97) && (buf[(ptr - 94) & BUFFSIZE] == 0x04)){
        if(1){
            __NOP();
        }        
    }
}

/*******************************************************************
                       Передача по UART1
*******************************************************************/
void UART1_sendbyte(uint8_t byte){
	while((MDR_UART1->FR & UART_FR_BUSY) == UART_FR_BUSY){}
	MDR_UART1->DR = byte;
}

/*******************************************************************
                       Передача по UART2
*******************************************************************/
void UART2_sendbyte(uint8_t byte){
	while((MDR_UART2->FR & UART_FR_BUSY) == UART_FR_BUSY){}
	MDR_UART2->DR = byte;
}

/*******************************************************************
                       Прерывание от UART1
*******************************************************************/
void UART1_IRQHandler(void){
    if(MDR_UART1->RIS & TXRIS){       //передача
        MDR_UART1->ICR |= (1<<5);
	}else if(MDR_UART1->RIS & RXRIS){ //прием
        uart1.ipRx = (uart1.ipRx + 1) & RXRING;
		uart1.RxBuf[uart1.ipRx & BUFFSIZE] = MDR_UART1->DR;
        MDR_UART1->ICR |= (1<<4);
    }
}

/*******************************************************************
                       Прерывание от UART2
*******************************************************************/
void UART2_IRQHandler(void){         
    if(MDR_UART2->RIS & TXRIS){       //передача
        MDR_UART2->ICR |= (1<<5);
	}else if(MDR_UART2->RIS & RXRIS){ //прием
        MDR_UART2->ICR |= (1<<4);
        uart2.ipRx = (uart2.ipRx + 1) & RXRING;
		uart2.RxBuf[uart2.ipRx & BUFFSIZE] = MDR_UART2->DR;
    }
}

/*******************************************************************
                   Инициализация модуля UART2
*******************************************************************/
void UART2_ini_PD0_PD1_std(t_BoudRate BoudRate){
    PORT_InitTypeDef GPIO_ini;
    UART_InitTypeDef UART2_ini;
    
    //RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
    
    GPIO_ini.PORT_Pin       = PORT_Pin_1;
    GPIO_ini.PORT_OE        = PORT_OE_OUT;
    GPIO_ini.PORT_PULL_UP   = PORT_PULL_UP_OFF;
    GPIO_ini.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    GPIO_ini.PORT_PD_SHM    = PORT_PD_SHM_OFF;
    GPIO_ini.PORT_PD        = PORT_PD_DRIVER;
    GPIO_ini.PORT_GFEN      = PORT_GFEN_OFF; 
    GPIO_ini.PORT_FUNC      = PORT_FUNC_ALTER;
    GPIO_ini.PORT_SPEED     = PORT_SPEED_MAXFAST;
    GPIO_ini.PORT_MODE      = PORT_MODE_DIGITAL;
    
    PORT_Init(MDR_PORTD, &GPIO_ini);
        
    GPIO_ini.PORT_Pin       = PORT_Pin_0;
    GPIO_ini.PORT_OE        = PORT_OE_IN;
    
    PORT_Init(MDR_PORTD, &GPIO_ini);
    
    RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);
   
    UART_BRGInit(MDR_UART2, UART_HCLKdiv1);    
        
    UART2_ini.UART_BaudRate             = BoudRate;
    UART2_ini.UART_FIFOMode             = UART_FIFO_OFF;
    UART2_ini.UART_HardwareFlowControl  = UART_HardwareFlowControl_TXE | UART_HardwareFlowControl_RXE;
    UART2_ini.UART_Parity               = UART_Parity_Even;
    UART2_ini.UART_StopBits             = UART_StopBits1;
    UART2_ini.UART_WordLength           = UART_WordLength8b;
    
    UART_Init(MDR_UART2, &UART2_ini); 
    
    UART_ITConfig(MDR_UART2, UART_IT_RX, ENABLE);
    NVIC_EnableIRQ(UART2_IRQn);
    
    UART_Cmd(MDR_UART2, DISABLE);
}

/*******************************************************************
                   Инициализация модуля UART1
*******************************************************************/
void UART1_ini_PB5_PB6_std(t_BoudRate BoudRate){
    PORT_InitTypeDef GPIO_ini;
    UART_InitTypeDef UART1_ini;
    
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
   
    GPIO_ini.PORT_Pin       = PORT_Pin_5;
    GPIO_ini.PORT_OE        = PORT_OE_OUT;
    GPIO_ini.PORT_PULL_UP   = PORT_PULL_UP_OFF;
    GPIO_ini.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    GPIO_ini.PORT_PD_SHM    = PORT_PD_SHM_OFF;
    GPIO_ini.PORT_PD        = PORT_PD_DRIVER;
    GPIO_ini.PORT_GFEN      = PORT_GFEN_OFF; 
    GPIO_ini.PORT_FUNC      = PORT_FUNC_ALTER;
    GPIO_ini.PORT_SPEED     = PORT_SPEED_MAXFAST;
    GPIO_ini.PORT_MODE      = PORT_MODE_DIGITAL;
    
    PORT_Init(MDR_PORTB, &GPIO_ini);
        
    GPIO_ini.PORT_Pin       = PORT_Pin_6;
    GPIO_ini.PORT_OE        = PORT_OE_IN;
    
    PORT_Init(MDR_PORTB, &GPIO_ini);
    
    RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);
   
    UART_BRGInit(MDR_UART1, UART_HCLKdiv1);    
        
    UART1_ini.UART_BaudRate             = BoudRate;
    UART1_ini.UART_FIFOMode             = UART_FIFO_OFF;
    UART1_ini.UART_HardwareFlowControl  = UART_HardwareFlowControl_TXE | UART_HardwareFlowControl_RXE;
    UART1_ini.UART_Parity               = UART_Parity_No;
    UART1_ini.UART_StopBits             = UART_StopBits1;
    UART1_ini.UART_WordLength           = UART_WordLength8b;
    
    UART_Init(MDR_UART1, &UART1_ini); 
    
    UART_ITConfig(MDR_UART1, UART_IT_RX, ENABLE);
    NVIC_EnableIRQ(UART1_IRQn);
    
    UART_Cmd(MDR_UART1, ENABLE);
}

