
#include "ads8865.h"

//0x40050000 CS
//0x44a10000 SPI_REG_AD
short ADS8865_sample(void) // wast at least 40us
{
	//xil_printf("STSTE_B = %x\r\n",*(u32*)0x44a10008);
	CS_CLR_AD;
	//xil_printf("STSTE_L = %x\r\n",*(u32*)0x44a10008);
	delay_us(1);
	SPI_REG_AD = 0x55555555;
	delay_us(5);
	CS_SET_AD;

	//xil_printf("%x\r\n",(u16)SPI_RECEIVE_AD);
	//xil_printf("%d\r\n",(int)(u16)SPI_RECEIVE_AD);
	return((short)((u16)SPI_RECEIVE_AD));
}
float ADS8865_short_deal(short AD_DATA)
{
	float v;
	/*
	if(AD_DATA > (u16)32767)
	{
		AD_DATA = (~AD_DATA) + 1;
		v =(float)(-1)* ((float)AD_DATA/ (float)32768)* VREF_AD;
	}
	else	v = ((float)(AD_DATA) / (float)32768 )*VREF_AD;*/
	v = (float)AD_DATA / (float)32768 * VREF_AD;
	return v;
}
float ADS8865_int_deal(int AD_DATA,int num)
{
	float v;
	v = (float)AD_DATA / (float)32768  / (float)num * VREF_AD;
	return v;
}
