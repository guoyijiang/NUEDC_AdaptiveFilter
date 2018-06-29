#include "rlcmeasure.h"
#include "delay.h"
#include "led.h"
int rlc_Initial(RLC_Measure* RLC)
{
//初始化Swith档位选择IO
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	//GPIOD
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	//GPIOG
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//
  GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	RLC->Rs[0] = 4.0130;
	RLC->Rs[1] = 36.0090;
	RLC->Rs[2] = 360.783;
	RLC->Rs[3] = 3580.483;
	RLC->Rs[4] = 35992.483;
	RLC->Rs[5] = 360119.483;
	RLC->Rs[6] = 3276399.48;
	
	RLC->autoMode = 0;
	RLC->isIt =0;
	RLC->freq = 1000.0f;
	
	RLC->RsState = 0;
	RLC->feature = 'R';
	
	SWITCH0 = 1;
	SWITCH1 = 0;
	SWITCH2 = 0;
	SWITCH3 = 0;
	SWITCH4 = 0;	
	SWITCH5 = 0;
	SWITCH6 = 0;
	SWITCHUX = 0;
	SWITCHUS = 0;
	return 0;
}

int rlc_SetFreq(RLC_Measure* RLC,double freq)
{
	u32 temp;
	RLC->freq = freq;
	temp = (u32)(freq*429.4967296);
	Fpga_WriteReg(AddrFreq,temp);
	return 0;
}

int changeXS(char target) //0 Ux 1 Us
{
		if(target == 0) SWITCHUX = 1;
		else SWITCHUX = 0;
		if(target == 1) SWITCHUS = 1;
		else SWITCHUS = 0;	
		return 0;
	
}
int rlc_SwitchDataXY(u32 xy)
{
	Fpga_WriteReg(AddrSwitchXY,xy);
	return 0;
}
double rlc_GetU(void)
{
	u16 i;
	u32 temp;
	double utemp;
	double sum =.0f;
	for(i=0;i<ADNSAMPLE;i++)
	{
		temp = Fpga_ReadReg(AddrRegAD);
		utemp = ((double)temp)/65536.0 * ADVREF;
		sum += utemp;
		delay_us(100);
	}
	
	//y = 1.1093170 x - 0.1219025 
	
	//y = 0.6001301 x + 0.7502003 

	utemp = (sum/ADNSAMPLE - 0.7502003)/0.6001301 + 0.00345; 
	//DebugStatePrint(5,"v= %f",utemp);
	return utemp;
}
int rlc_GetResult(RLC_Measure* RLC)
{
	double utemp;
	
	changeXS(0);
	rlc_SwitchDataXY(0);
	delay_ms(50);
	RLC->Uxx = rlc_GetU();
	
	rlc_SwitchDataXY(1);
	delay_ms(2);
	RLC->Uxy = rlc_GetU();
	
	changeXS(1);
	rlc_SwitchDataXY(0);
	delay_ms(50);
	RLC->Usx = rlc_GetU();
	
	rlc_SwitchDataXY(1);
	delay_ms(2);
	RLC->Usy = rlc_GetU();	
	
	return 0;
}
int rlc_Calc(RLC_Measure* RLC)
{
	double father = .0f;
	father = ((RLC->Usx * RLC->Usx) + (RLC->Usy * RLC->Usy)) / RLC->Rs[RLC->RsState] * -1.0f;
	RLC->Zre = ((RLC->Uxx * RLC->Usx) + (RLC->Uxy * RLC->Usy))/father;
	RLC->Zim = ((RLC->Uxy * RLC->Usx) - (RLC->Uxx * RLC->Usy))/father;
	RLC->Zabs = sqrt((RLC->Zre * RLC->Zre)+(RLC->Zim * RLC->Zim));
	RLC->QD = fabs(RLC->Uxx / RLC->Uxy);
	if(fabs(RLC->Zre) > fabs(RLC->Zim)*4.0) RLC->feature = 'R';
		else if(RLC->Zim > 0.0f) RLC->feature = 'L';
			else RLC->feature = 'C';
	return 0;
}

int rlc_UpdataPlay(RLC_Measure* RLC)
{
	double R,L,C;
	char charR,charC,charL;
	
//	R = RLC->Zre /1.024;
	R = RLC->Zre;
	if(R > 1000.0)
	{
		R = R/1000.0;
		charR = 'k';
	}
	else charR = ' ';

	
	L = fabs(RLC->Zim /6.2831853071795864769/RLC->freq);
	if(L > 0.001) 
	{
		L = L*1000.0;
		charL = 'm';
	}
	else if(L > 0.000001) 
	{
		L = L*1000000.0;
		charL = 'u';
	}	
	
	
//	C = fabs(1.0/6.2831853071795864769/ RLC->freq / RLC->Zim/1.25);
		C = fabs(1.0/6.2831853071795864769/ RLC->freq / RLC->Zim);
	if(C> 0.001) charC = 'm';
	else if(C > 0.000001) 
	{
		C = C*1000000.0;
		charC = 'u';
	}
	else if(C > 0.000000001) 
	{
		C = C*1000000000.0;
		charC = 'n';
	}	
	else if(C > 0.000000000001) 
	{
		C = C*1000000000000.0;
		charC = 'p';
	}
	else charC = ' ';
	
//	DebugStatePrint(0,"uxx= %9f  uxy= %9f",RLC->Uxx,RLC->Uxy);
//	DebugStatePrint(1,"usx= %9f  usy= %9f",RLC->Usx,RLC->Usy);
//	DebugStatePrint(2,"z= %11.3ef  zre= %11.3ef  zim=%11.3ef",RLC->Zabs,RLC->Zre,RLC->Zim);	
//	DebugStatePrint(3,"feature=%c  QD=%.3ef",RLC->feature,RLC->QD);	
//	DebugStatePrint(4,"R=%11.3ef %cOhm",R,charR);
//	DebugStatePrint(5,"L=%11.3ef %cH",L,charL);
//	DebugStatePrint(6,"C=%11.3ef %cF",C,charC);
//	
//	DebugStatePrint(7,"f=%.1f  XSstate=%d  Rs=%.3ef",RLC->freq,RLC->RsState,RLC->Rs[RLC->RsState]);
	
	DebugStatePrint(0,"z= %11.3ef  zre= %11.3ef  zim=%11.3ef",RLC->Zabs,RLC->Zre,RLC->Zim);	
	DebugStatePrint(1,"feature=%c  QD=%.3ef",RLC->feature,RLC->QD);	
	DebugStatePrint(2,"R=%11.3ef %cOhm",R,charR);
	DebugStatePrint(3,"L=%11.3ef %cH",L,charL);
	DebugStatePrint(4,"C=%11.3ef %cF",C,charC);
	
	DebugStatePrint(5,"f=%.1f  XSstate=%d  Rs=%.3ef",RLC->freq,RLC->RsState,RLC->Rs[RLC->RsState]);
	
	if(RLC->isIt) LED0 = 1;
	else LED0 = 0;
	
	return 0;
}
//return 1表示测量Z太小 2表示Z太大 3表示量程转换了
int rlc_ChangeRange(RLC_Measure* RLC)
{
	int i;
	int lastState = RLC->RsState;
	
	for(i=0;i < NRS;i++)
	{
		if(i ==0) 
		{
			if( (RLC->Zabs > 0)&&(RLC->Zabs < RLC->Rs[1]/2.3)) RLC->RsState = 0;
		}
		else if(i == 6)
		{
			if( RLC->Zabs > RLC->Rs[6]/2.3) RLC->RsState = 6;
		}
		else
		if( (RLC->Zabs > RLC->Rs[i]/2.3)&&(RLC->Zabs < RLC->Rs[i+1]/2.3) )
		{
				RLC->RsState = i;
				break;
		}
	}
	//RLC->RsState =3;
//	 if((RLC->Zabs > 0)&&(RLC->Zabs < RLC->Rs[0]/2.3*1.1)) RLC->RsState =0;
//	 if((RLC->Zabs > RLC->Rs[0]/2.3*0.9)&&(RLC->Zabs < RLC->Rs[1]/2.3*1.1)) RLC->RsState =1;
//	 if((RLC->Zabs > RLC->Rs[1]/2.3*0.9)&&(RLC->Zabs < RLC->Rs[2]/2.3*1.1)) RLC->RsState =2;
//	 if((RLC->Zabs > RLC->Rs[2]/2.3*0.9)&&(RLC->Zabs < RLC->Rs[3]/2.3*1.1)) RLC->RsState =3;
//	 if((RLC->Zabs > RLC->Rs[3]/2.3*0.9)&&(RLC->Zabs < RLC->Rs[4]/2.3*1.1)) RLC->RsState =4;
//	 if((RLC->Zabs > RLC->Rs[4]/2.3*0.9)&&(RLC->Zabs < RLC->Rs[5]/2.3*1.1)) RLC->RsState =5;
//	 if((RLC->Zabs > RLC->Rs[5]/2.3*0.9)&&(RLC->Zabs < RLC->Rs[6]/2.3*1.1)) RLC->RsState =6;
	
	if(lastState == RLC->RsState)
	{
		RLC->isIt = 1;
		return 0;
	}
	else 
	{
	 RLC->isIt = 0;
	 changeState(RLC->RsState);
	 return 3;
	}
}

int changeState(char target)
{
		if(target == 0) SWITCH0 = 1;
		else SWITCH0 = 0;
		if(target == 1) SWITCH1 = 1;
		else SWITCH1 = 0;
		if(target == 2) SWITCH2 = 1;
		else SWITCH2 = 0;
		if(target == 3) SWITCH3 = 1;
		else SWITCH3 = 0;
		if(target == 4) SWITCH4 = 1;
		else SWITCH4 = 0;
		if(target == 5) SWITCH5 = 1;
		else SWITCH5 = 0;
		if(target == 6) SWITCH6 = 1;
		else SWITCH6 = 0;		
		return 0;
}

//return 2auto 1freqchange 0keep
int rlc_ChangeFreq(RLC_Measure* RLC,char inputChar)
{
//	if(RLC->isIt == 1) 
//	{
//		/*if(RLC->autMode == 1)
//		{
//			rlc_SetFreq(RLC, 10000.0f);
//			RLC->autMode = 0;
//			return 2;
//		}*/
//	 // else if(inputChar == 1) RLC->autMode =1;
//		return 2;	
//	}
	if(inputChar == 0) return 0;
	else if(inputChar == 1) //设定频率 A=auto F=1k G=10k H=100k
		RLC->autoMode =1;
	else if(inputChar == 2)
		rlc_SetFreq(RLC, 1000.0);
	else if(inputChar == 3)
		rlc_SetFreq(RLC, 10000.0);
	else if(inputChar == 4)
		rlc_SetFreq(RLC, 100000.0);
	return 1;
}

int rlc_Measure(RLC_Measure* RLC,char key)
{
	

	rlc_GetResult(RLC);//获取测量数据~100ms
	rlc_Calc(RLC);//计算结果并判断
	rlc_UpdataPlay(RLC);//更新显示
	rlc_ChangeRange(RLC);//量程转换
	rlc_ChangeFreq(RLC,key);//频率重设 留时间适应
//	RLC->RsState =0;
//	changeState(0);
	return 0;
}

//WREG[7] 为32位有符号数 0~65536 对应0~1倍幅度；
//WREG[8]为32位有符号数 -512~511为偏置调整范围，对应-Vref~Vref；
//lift -1~1
int setFDA(u32 amp,int lift)
{
	Fpga_WriteReg(FDAAMPREG,amp);
	Fpga_WriteReg(FDALIFTREG,lift);
	
}

//WREG[5] 为32位有符号数 0~65536 对应0~1倍幅度；
//WREG[6]为32位有符号数 -512~511为偏置调整范围，对应-Vref~Vref；
//lift -DAVREF ~ DAVREF
int setDAC8811(u32 amp,int lift)
{
	
	Fpga_WriteReg(DAC8811AMPREG,amp);
	Fpga_WriteReg(DAC8811LIFTREG,lift);
	
}
