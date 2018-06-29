//hard
#include "dac8811.h"

int dac8811_Hard_Write(u16 data)
{
	
	Fpga_WriteReg(DAC8811REG,data);
	return 0;
}

int dac8811_Hard_SetVoltage(double v)
{
	u32 temp;
	temp = (u32)((v/DAC8811VREF + 1.0)*32768 - 0.5);
	if(temp > 65535) temp = 65535;
	dac8811_Hard_Write((u16)temp);
	
	return 0;
}
