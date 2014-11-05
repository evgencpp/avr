#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <avr/eeprom.h>

#define CONFIG_DVG_CNT      2       // drive count
/*
typedef struct __tagdrivecfg_t{
	uint8_t		dvgDir;
    uint8_t		encDir;
	uint16_t	minPWM;
	uint16_t	maxPWM;
}drivecfg_t;
*/
typedef struct __tagdrivecfg_t{
	uint8_t			drvDir;				// drive direction
	uint8_t			encDir;				// encoder direction
    uint16_t		maxI;				// maximum current
    uint16_t		maxPWM_CW;			// maxPWM_CW
    uint16_t		maxPWM_CCW;			// maxPWM_CCW
	uint16_t		minPWM_CW;			// minPWM_CW
	uint16_t		minPWM_CCW;			// minPWM_CW
	
	float			spKp;
	float			spKi;
	float			spKd;
	
	float			posKp;
	float			posKi;
	float			posKd;
}drivecfg_t;

/*
{"n=drvDir;     t=byte; c=1;    a=0;    hw=1;   e=dropdown['CW', 'CCW']; hit=Drive direction CW/CCW;"},
    {"n=encDir;     t=byte; c=1;    a=0;    hw=1;   e=radio['CW', 'CCW']; hit=Encoder direction CW/CCW;"},
    {"n=maxI;       t=word; c=1;    a=0;    hw=1;   e=edit; hit=MAX current;"},
    {"n=maxPWM_CW;  t=word; c=1;    a=0;    hw=1;   e=edit;"},
    {"n=maxPWM_CCW; t=word; c=1;    a=0;    hw=1;   e=edit;"},
    {"n=encCnt;     t=word; c=1;    a=0;    hw=0;   e=edit;"},
    {"n=gearK;      t=float; c=1;   a=0;    hw=0;   e=edit;"},

    {"n=minPWM_CW;  t=word; c=1;    a=1;    hw=1;   e=edit;"},
    {"n=minPWM_CCW; t=word; c=1;    a=1;    hw=1;   e=edit;"},

    {"n=Speed PID params;       t=byte; c=1;    a=0;    hw=0;   e=delimiter;"},
    {"n=Kp;   t=float; c=1;    a=1;    hw=1;   e=edit;"},
    {"n=Ki;   t=float; c=1;    a=1;    hw=1;   e=edit;"},
    {"n=Kd;   t=float; c=1;    a=1;    hw=1;   e=edit;"},

    {"n=Pos PID params;       t=byte; c=1;    a=0;    hw=0;   e=delimiter;"},
    {"n=Kp;   t=float; c=1;    a=1;    hw=1;   e=edit;"},
    {"n=Ki;   t=float; c=1;    a=1;    hw=1;   e=edit;"},
    {"n=Kd;   t=float; c=1;    a=1;    hw=1;   e=edit;"},
*/

#define DRV_CFG_SIZE	(sizeof(drivecfg_t))

unsigned char loadCfg(void);
void LoadConfig(uint8_t idx);
void SaveConfig(uint8_t idx);
unsigned char readCfg(const drivecfg_t *cfg);
unsigned char writeCfg(const drivecfg_t cfg);
void SetDefConfig(void);
extern drivecfg_t curConfig[CONFIG_DVG_CNT];

#endif
