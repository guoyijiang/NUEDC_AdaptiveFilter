#include "lcdpro.h"
#define PRINT_LEN 38
char print_temp[100];
void LCD_Debug_ShowString(u8 *p)
{
	static u16 x = 8;
	static u16 y = 16;
	while(*p!='\0')
	{
		if(x>468){x = 8; y += 24;}
		if(y>768){y = 16; LCD_Fill(8,16,472,784,WHITE);}
		if((*p<='~')&&(*p>=' ')) {LCD_ShowChar(x,y,*p,16,0);x+=8;}
		else if(*p == '\t') x += 32;
		else if( *p == '\r') {x = 8;y += 24;}
		else if(*p == '\n')
			if(*(p-1) != '\r') y+=24;
		p++;
	} 
	
}
void DebugPrintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    //按格式存储到临时区，然后lcdprintf
    vsprintf(print_temp,format, args);
    va_end(args);
		LCD_Debug_ShowString((u8*)print_temp);
}
void DebugStatePrint(u32 member,const char *format, ...)
{
		u16 yb;
		u16 ye;
	  va_list args;
    va_start(args, format);
    //按格式存储到临时区，然后lcdprintf
    vsprintf(print_temp,format, args);
    va_end(args);
		//LCD_Debug_ShowString((u8*)print_temp);

		if(member<15)
		{
		yb = 16 + 24 * member;
		ye = yb + 24;
		LCD_Fill( 8, yb, 468, ye, BACK_COLOR);
		LCD_ShowString(8,yb,460,16,16,(u8*)print_temp );
			
		}
	
}
void LCD_ShowStringCentre(u16 x1, u16 y1,u16 x2, u16 y2,u8 size,u8 *p)
{
		u8 length;
		u16 xs;
		u16	ys;
		length = strlen(p);
		xs = (x1+x2)/2 - length*size/4;
		ys = (y1+y2)/2 - size/2;

    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        LCD_ShowChar(xs,ys,*p,size,0);
        xs+=size/2;
        p++;
    }  
		
}

