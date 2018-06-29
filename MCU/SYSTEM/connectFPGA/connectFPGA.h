#ifndef __CONNECTFPGA_H
#define __CONNECTFPGA_H

#include "sys.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_gpio.h"
#include "delay.h"
#include "lcdpro.h"
#define	FPGA_CS 		PGout(8)  		//CS

void SPI1_Init(void);
u16 SPI1_ReadWriteByte(u16 TxData);
void Fpga_init(void);
void Fpga_WriteReg(u16 addr,u32 data);
u32 Fpga_ReadReg(u16 addr);
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler);


#endif
