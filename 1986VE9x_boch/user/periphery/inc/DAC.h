#ifndef DAC_H
#define DAC_H

#include "mdr32fx.h"
#include "types.h"
#include "MDR32F9Qx_port.h"

typedef enum {DAC1 = 1, DAC2 = 2} t_DAC_chan;

/*******************************************************************
                            Прототипы
*******************************************************************/
void DAC_init(void);
void DAC_Cmd(t_DAC_chan DAC, FunctionalState NewState);
void DAC_SetData(t_DAC_chan DAC, uint32_t Data);

#endif
