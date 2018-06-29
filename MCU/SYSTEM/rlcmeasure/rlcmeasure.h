#ifndef __RLCMEASURE_H
#define __RLCMEASURE_H
#include "sys.h"
#include "math.h"
#include "connectFPGA.h"
#include "lcd.h"
#include "lcdpro.h"

#define AddrFreq 2
#define AddrSwitchXY 4

#define DAC8811AMPREG 5
#define DAC8811LIFTREG 6


#define FDAAMPREG 7
#define FDALIFTREG 8


#define AddrRegAD 12


#define VLIFT 1.247
#define ADVREF 3.0
#define DAVREF 2.5000
#define ADNSAMPLE 100

#define NRS 7

//#define	SWITCH6 		PDout(11) 
//#define	SWITCH1 		PDout(12)
//#define	SWITCH0 		PDout(13) 
//#define	SWITCH5 		PGout(2)
//#define	SWITCH4 		PGout(3) 
//#define	SWITCH3 		PGout(4) 
//#define	SWITCH2 		PGout(5) 
//#define	SWITCHUX		PGout(6) 
//#define	SWITCHUS 		PGout(7)


#define	SWITCHUS 		PDout(11) 
#define	SWITCHUX		PDout(12)
#define	SWITCH6 		PDout(13) 
#define	SWITCH1 		PGout(2)
#define	SWITCH0 		PGout(3) 
#define	SWITCH5 		PGout(4) 
#define	SWITCH4 		PGout(5) 
#define	SWITCH3 		PGout(6) 
#define	SWITCH2 		PGout(7)


typedef struct 
{
	
	double Uxx;
	double Uxy;
	double Usx;
	double Usy;
	
	double Zabs;
	double Zre;
	double Zim;
	double QD;
	
	double freq;
	int RsState;
	double Rs[NRS];
	
	char feature; // R L C 
	char isIt;
	char autoMode;
	
}RLC_Measure;

int rlc_Initial(RLC_Measure* RLC);

int rlc_SetFreq(RLC_Measure* RLC,double freq);

int changeXS(char target);//0 Ux 1 Us 
int rlc_SwitchDataXY(u32 xy);//xy = 0:X   1:Y  
int changeState(char target); 

double rlc_GetU(void); 
int rlc_GetResult(RLC_Measure* RLC);

int rlc_Calc(RLC_Measure* RLC);
int rlc_UpdataPlay(RLC_Measure* RLC);

int rlc_ChangeRange(RLC_Measure* RLC);
int rlc_ChangeFreq(RLC_Measure* RLC,char inputChar);

int rlc_Measure(RLC_Measure* RLC,char inputChar);

int setFDA(u32 amp,int lift);
int setDAC8811(u32 amp,int lift);

#endif
