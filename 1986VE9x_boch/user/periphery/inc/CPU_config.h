#ifndef CPU_CONFIG_H
#define CPU_CONFIG_H

typedef enum {MHz_8, MHz_16, MHz_32, MHz_48, MHz_64, MHz_80, MHz_90} t_CPU_mode;

/*******************************************************************
                            Прототипы
*******************************************************************/
void Clk_64MHz_Init(void);

#endif
