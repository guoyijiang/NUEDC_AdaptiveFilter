#ifndef __OSCI_H
#define __OSCI_H
#include "sys.h"
#include "touch.h"
#define OSCISTATEREG 12
#define OSCIDATAREG 13
#define OSCIENREG 3

#define OSCIDATALENGTH 256
#define OSCIDATARANGE 256

#define OSCILENGTH 600 //为20整数倍
#define OSCIHIGH 400 //256.1.5 为8整数倍

#define OSCISX 0
#define OSCISY 0

#define OSCIARRAYLENGTH  120000 //400*600

//{26'b0,ReadFifoState,WriteFifoState,stack_empty,stack_full}
typedef struct
{
	u8 readsState;
	u8 writeState;
	u8 empty;
	u8 full;
}_osci;
typedef struct
{
	u16 x;
	u16 y;
}_oscipoint;


int osci_Init(void);
int osci_Off(void);
int osci_DrawBack(void);
int osci_DrawGrid(void);
int osci_DrawData(void);

int osci_DrawUI(void);

int osci_Sample(void);
int osci_Read(void);
int osci_GetState(void);
int osci_SetLinable(u8);
int osci_DrawPointToArray(u16 x,u16 y);
int osci_DrawLineToArray(u16 x1, u16 y1, u16 x2, u16 y2);
int osci_PrintToArray(void);

u8 osci_KeyScan(void);
int osci_KeyDeal(u8 keyvalue);


#endif
