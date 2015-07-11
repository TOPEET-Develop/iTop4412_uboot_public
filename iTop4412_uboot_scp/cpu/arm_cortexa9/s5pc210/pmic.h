/**************************************************************************************
* 
*	Project Name : S5PC200 Validation
*
*	Copyright 2010 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S5PC200.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : pmic.h
*  
*	File Description : PMIC test
*
*	Author : GiYoung Yoo
*	Dept. : AP Development Team
*	Created Date : 2010/7/6
*	Version : 0.1 
* 
*	History
*	- Created(GiYoung Yoo 2010/7/6)
*  
**************************************************************************************/
#ifndef __PMIC_H__
#define __PMIC_H__

#define MAX8649_ADDR    0xc0    // VDD_INT -I2C1
#define MAX8649A_ADDR   0xc4    //VDD_G3D - I2C0
#define MAX8952_ADDR    0xc0    //VDD_ARM - I2C0

#define MAX8997_ADDR	0xcc    //TA4 PMU- I2C1


#define MAX8997_BUCK1_CTRL  (0x18) 
#define MAX8997_BUCK1_DVS	(0x19)


#define MAX8997_BUCK2_CTRL  (0x21) 
#define MAX8997_BUCK2_DVS	(0x22)


#define MAX8997_BUCK3_CTRL  (0x2a) 
#define MAX8997_BUCK3_DVS	(0x2b)




#define MAX8997_BUCK4_CTRL  (0x2c) 
#define MAX8997_BUCK4_DVS	(0x2d)

#define MAX8997_BUCK5_CTRL  (0x2e) 
#define MAX8997_BUCK5_DVS	(0x2f)

#define MAX8997_BUCK6_CTRL  (0x37) 


//LDO control
/*
LDO 1: 3.3V
Bit[7:6]
Enable Mode Control.
00: Output OFF (regardless of PWREN)
01: Output ON/OFF controlled by PWREN
PWREN=1: Output ON in normal mode
PWREN=0: Output OFF
10: Output On with Green Mode by PWREN
[ PWREN=1 : Output On with Normal Mode ]
[ PWREN=0: Output On with Low Power Mode ]
11: Output ON with Normal Mode (regardless of PWREN)
*/
#define MAX8997_LDO1_CTRL (0x3b) 

/*
LDO 2: 1.1V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO2_CTRL (0x3c) 
/*
LDO 3: 1.1V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO3_CTRL (0x3d) 

/*
LDO 4: 1.8V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO4_CTRL (0x3e) 
/*
LDO 5: 1.2V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO5_CTRL (0x3f) 

/*
LDO 6: 1.8V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO6_CTRL (0x40) 
/*
LDO 7: 1.8V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO7_CTRL (0x41) 
/*
LDO 8: 3.3V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO8_CTRL (0x42) 

/*
LDO 9: 2.8V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO9_CTRL (0x43) 

/*
LDO 10: 1.1V
Enable Mode Control.
00: Output OFF (regardless of PWREN)
01: Output ON/OFF controlled by PWREN
PWREN=1: Output ON in normal mode
PWREN=0: Output OFF
10: Output On with Green Mode by PWREN
[ PWREN=1 : Output On with Normal Mode ]
[ PWREN=0: Output On with Green Mode ]
11: Output ON with Normal Mode (regardless of PWREN)


*/
#define MAX8997_LDO10_CTRL (0x44) 

/*
@@@default is off
LDO 11: 2.8V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO11_CTRL (0x45) 
/*
@@@default is off
LDO 12: 1.2V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO12_CTRL (0x46) 

/*
@@@default is off
LDO 13: 2.8V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO13_CTRL (0x47) 

/*
@@@default is off
LDO 14: 1.8V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO14_CTRL (0x48) 
/*
@@@default is off
LDO 15: 2.8V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO15_CTRL (0x49) 
/*
@@@default is off
LDO 16: 3.3V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO16_CTRL (0x4a) 
/*
@@@default is off
LDO 17: 3.3V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO17_CTRL (0x4b) 
/*
@@@default is off
LDO 18: 3.3V
Enable Mode Control
00: Output off
01: Output On with Green Mode
10: Output On
with Green Mode if PWREN=0.
with Normal Mode if PWREN=1.
11: Output On with Normal Mode

*/
#define MAX8997_LDO18_CTRL (0x4c) 
/*
LDO 21: 1.2V

Enable Mode Control.
00: Output OFF (regardless of PWREN)
01: Output ON/OFF controlled by PWREN
PWREN=1: Output ON in normal mode
PWREN=0: Output OFF
10: Output On with Green Mode by PWREN
[ PWREN=1 : Output On with Normal Mode ]
[ PWREN=0: Output On with Green Mode ]
11: Output ON with Normal Mode (regardless of PWREN)
*/
#define MAX8997_LDO21_CTRL (0x4d) 

typedef enum
{
	eSingle_Buck ,
	eMAXIM ,
	eNS ,
} PMIC_MODEL_et;

typedef enum
{
	eVDD_ARM ,
	eVDD_INT ,
	eVDD_G3D ,
	TA4_PMU  ,
} PMIC_et;

typedef enum
{
	eVID_MODE0 ,
	eVID_MODE1 ,
	eVID_MODE2 ,
	eVID_MODE3,
} PMIC_MODE_et;

typedef struct {
	char*	name;           //voltage name
	u8	ucDIV_Addr;     //device address
	u8	ucI2C_Ch;       //iic ch
}PMIC_Type_st; 

extern void PMIC_InitIp(void);

#ifndef CONFIG_TA4
void PMIC_SetVoltage_Buck(PMIC_et ePMIC_TYPE ,PMIC_MODE_et uVID, float fVoltage_value );
#else
void PMIC_SetVoltage_Buck(PMIC_et ePMIC_TYPE ,u8 addr, u8 value );
void PMIC_GetVoltage_Buck(PMIC_et ePMIC_TYPE ,u8 addr, u8 *value );

#endif

#endif //__PMIC_H__

