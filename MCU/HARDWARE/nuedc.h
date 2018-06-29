#ifndef __NUEDC_H
#define __NUEDC_H

#include "sys.h"
#include "math.h"
#include "connectFPGA.h"
#include "lcd.h"
#include "lcdpro.h"

#define PI 3.141592653589793238467
#define REGTCNT 11 //r
#define REGSIN 12  //r
#define REGCOS 13 //r
#define REGSHIFT 1 //w


#define SWCHA 2 //w
#define SWCHB 3 //w

#define HDASHIFT 4 //w  0

#define DSNSHIFT 6 //signed32 
#define NOSESHIFT 7 //signed32 

#define TRIMODEREG 8 // 1:软件触发   0:默认硬件触发

int nuedc_Read();
int nuedc_ChangePhaseMode(u8);
int nuedc_Write(u32 x);
int nuedc_Display();
int nuedc_switchA(u32 x);
int nuedc_switchB(u32 x);
#endif
