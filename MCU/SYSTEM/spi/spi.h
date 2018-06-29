#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

							  
void SPI3_Init(void);			 //初始化SPI3口
void SPI3_SetSpeed(u8 SpeedSet); //设置SPI3速度   
u16 SPI3_ReadWriteByte(u16 TxData);//SPI3总线读写一个字节
		 
#endif

