#ifndef UART_H
#define UART_H 

#include "mdr32fx.h"
#include "types.h"

#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

#define OERIS				((uint16_t)(1 << 10))		// Прерывание от fifo 
#define TXRIS				((uint16_t)(1 << 5))		// Прерывание от передатчика	
#define RXRIS				((uint16_t)(1 << 4))		// Прерывание от приемника
#define BUFFSIZE		    ((uint8_t)0xff)
#define RXRING              ((uint8_t)0xff)                            
#define TXRING              ((uint8_t)0xff)                              
#define MPV_BUSY            ((uint16_t)0x0010)          // Оперативные признаки навигационно-временного решения - bit4 

typedef enum {BoudRate_9600 = 9600, BoudRate_57600 = 57600, BoudRate_38400 = 38400, BoudRate_115200 = 115200} t_BoudRate;

typedef struct{
	uint8_t TxBuf[256];
	uint8_t upTx;
	uint8_t ipTx;
	uint8_t RxBuf[256];
	uint8_t upRx;
	uint8_t ipRx;            
} _UART_TypeDef;

/*******************************************************************
                            Объявления
*******************************************************************/
extern volatile _UART_TypeDef uart1, uart2;
extern          uint16_t mpv_ready;

/*******************************************************************
                            Объявления
*******************************************************************/
void UART1_sendbyte(uint8_t byte);
void UART2_sendbyte(uint8_t byte);
void UART_ProcessMessage(volatile uint8_t* buf, uint8_t ptr);
void UART2_ini_PD0_PD1_std(t_BoudRate BoudRate);
void UART1_ini_PB5_PB6_std(t_BoudRate BoudRate);

#endif
