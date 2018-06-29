#include "nuedc.h"


double T = 1.0; //us
double f = .0;  //khz
double sinx  = .0;
double cosx  = .0;
double fyx   = .0;

u32 Tcnt = 1;
u32 phaseShift = 0;
u8 phaseMode = 1;
int nuedc_Read()
{
	u32 temp;double temp2;int temp3;
	Tcnt = Fpga_ReadReg(REGTCNT);
	T = (double)Tcnt / 100.0 ;
	f = 100000.0/Tcnt;
	temp3 = (int)Fpga_ReadReg(REGSIN) + 4;
	sinx = (double)temp3;
	temp3 = (int)Fpga_ReadReg(REGCOS) + 4;
	cosx = (double)temp3;
	temp2 = atan2(sinx,cosx)*180.0/PI; // -180~180
	if(temp2 > .0) fyx = temp2;
	else fyx = 360.0 + temp2;
	return 0;
}	
int nuedc_Write(u32 x) //1 auto
{
	u32 temp;
	if(phaseMode == 1)
	{
		phaseShift = (u32)(fyx/360.0*Tcnt);
		Fpga_WriteReg(REGSHIFT,phaseShift);		
	}
	else 
	{
		phaseShift = x;
		Fpga_WriteReg(REGSHIFT,phaseShift);		
	}

	return 0;
}
int nuedc_ChangePhaseMode(u8 x)
{
	phaseMode = x;
	return 0;
}
int nuedc_Display()
{
	DebugStatePrint(2,"T=%7.3f us  f=%7.3f kHz",T,f);
	DebugStatePrint(3,"sinx=%.1f  cosx=%.1f",sinx,cosx);
	DebugStatePrint(4,"phase=%.1f  shiftCnt=%d",fyx,phaseShift);
	return 0;
}
int nuedc_switchA(u32 x)
{
	Fpga_WriteReg(SWCHA,x);
	return 0;
}
int nuedc_switchB(u32 x)
{
	Fpga_WriteReg(SWCHB,x);
	return 0;
}
