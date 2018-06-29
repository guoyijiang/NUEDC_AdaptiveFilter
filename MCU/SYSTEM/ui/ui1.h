#ifndef __UI1_H
#define __UI1_H

#include "sys.h"
#include "led.h"
#include "lcd.h"
#include "lcdpro.h"
#include "touch.h"
#include "string.h"

#define NKEY1	 5
#define WITHKEY1 77 //ºáÏò6¸ö
#define HIGHKEY1 50 //
typedef struct key1
{
	u16 num;
	u16 xb;
	u16 xe;
	u16 yb;
	u16 ye;
	char name[10];
	
}KEY1STRUCT;

int keyboard1_Input(KEY1STRUCT *key1array);
int keyBoard1_Generate(KEY1STRUCT *key1array);


#endif
