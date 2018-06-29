#ifndef __LCDPRO_H
#define __LCDPRO_H	
#include "sys.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "lcd.h"

#define BUTTONNUM 16
#define STATENUM 8
#define STATELENGTH
#define JUDGEKEY(x1,y1,x2,y2,x,y) (((x)>(x1))&&((x)<(x2))&&((y)>(y1))&&((y)<(y2)))

typedef struct 
{
	
	char* ButtonName[BUTTONNUM];
	void (*ButnHandler[BUTTONNUM])(void);
	
	char* StateName[STATENUM];
	char State[STATELENGTH];
	
}UI1_Typedef;

void DebugPrintf(const char *format, ...);
void LCD_Debug_ShowString(u8 *p);
void DebugStatePrint(u32 member,const char *format, ...);

//适合画按键的名称，单行，注意字符串不要过长
void LCD_ShowStringCentre(u16 x1, u16 y1,u16 x2, u16 y2,u8 size,u8 *p); 

//用于判断按键位置

#endif
