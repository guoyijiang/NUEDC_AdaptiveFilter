#include <math.h>
#include <stdio.h>
#include "delay.h"

#define SPI_REG_AD *(u32*)0x44a10000
#define SPI_RECEIVE_AD *(u32*)0x44a10004
#define SPI_STATE_AD *(u32*)0x44a10008
#define CS_SET_AD *(u32*)0x40050000 = 0x03
#define CS_CLR_AD *(u32*)0x40050000 = 0x02
#define VREF_AD  (float)2.50;

short ADS8865_sample(void);
float ADS8865_short_deal(short AD_DATA);
float ADS8865_int_deal(int AD_DATA,int num);
