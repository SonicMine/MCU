#ifndef PORTS_H
#define PORTS_H

#include "mdr32fx.h"
#include "types.h"
#include "MDR32F9Qx_it.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

/*******************************************************************
                            Прототипы
*******************************************************************/
void PortA_Init(void);
void PortC_Init(void);
void PortD_Init(void);
void PortD_Test(void);
void Ext_Int1_Init(void);
void Ext_Int2_Init(void);
void Ext_Int3_Init(void);
void Ext_Int4_Init(void);

#endif
