#ifndef __DAC8811_H
#define __DAC8811_H
#include "connectFPGA.h"
#define DAC8811REG 4
#define DAC8811VREF 2.50

#include "sys.h"

int dac8811_Hard_Write(u16 data);
int dac8811_Hard_SetVoltage(double v);

#endif
