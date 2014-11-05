/*
 * drive.c
 *
 * Created: 04.04.13 0:16:58
 *  Author: ЖЕНЯ
 */ 

#include <avr/io.h>
#include "drive.h"
#include "config.h"


void InitDrive(drive_pins_t ioctrl)
{
	*ioctrl.ADDR |= (1<<ioctrl.AHI) | (1<<ioctrl.ALO);
	*ioctrl.BDDR |= (1<<ioctrl.BHI) | (1<<ioctrl.BLO);
	*ioctrl.MRSTDDR |= (1<<ioctrl.MRST);
	

	// fast PWM, mode 14, TOP-ICR, prescaler=8
	*ioctrl.TCCRA_A = 0x0000;
	// COM1A1 COM1A0 COM1B1 COM1B0 COM1C1 COM1C0 WGM11 WGM10
	*ioctrl.TCCRA_A |= (1 << WGM11);
	// ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
	*ioctrl.TCCRB_A |= (1 << WGM13) | (1<<WGM12) | (1 << CS11);
	*ioctrl.ICR_A = 0x80;	// freq 15.5 kHz
	*ioctrl.OCRA_A = 0x0;
	
	// fast PWM, mode 14, TOP-ICR, prescaler=8
	*ioctrl.TCCRA_B = 0x0000;
	// COM1A1 COM1A0 COM1B1 COM1B0 COM1C1 COM1C0 WGM11 WGM10
	*ioctrl.TCCRA_B |= (1 << WGM11);
	// ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
	*ioctrl.TCCRB_B |= (1 << WGM13) | (1<<WGM12) | (1 << CS11);
	*ioctrl.ICR_B = 0x80;	// freq 15.5 kHz
	*ioctrl.OCRA_B = 0x0;
	
	DriveRst(ioctrl);
	DriveWind(ioctrl);
}

void DriveRst(drive_pins_t ioctrl)
{
	*ioctrl.TCCRA_A &= ~(1<<COM1A1);
	*ioctrl.TCCRA_B &= ~(1<<COM1A1);
	*ioctrl.MRSTPORT &= ~(1<<ioctrl.MRST);
}

void DriveWind(drive_pins_t ioctrl)
{
	if(ioctrl.wind == 0x0){
		*ioctrl.MRSTPORT |= (1<<ioctrl.MRST);
		*ioctrl.APORT &= ~(1<<ioctrl.AHI);
		*ioctrl.BPORT &= ~(1<<ioctrl.BHI);
		*ioctrl.APORT &= ~(1<<ioctrl.ALO);
		*ioctrl.BPORT &= ~(1<<ioctrl.BLO);
	} else {
		*ioctrl.MRSTPORT |= (1<<ioctrl.MRST);
		*ioctrl.APORT |= (1<<ioctrl.AHI);
		*ioctrl.BPORT |= (1<<ioctrl.BHI);
		*ioctrl.APORT &= ~(1<<ioctrl.ALO);
		*ioctrl.BPORT &= ~(1<<ioctrl.BLO);
	}
}

void UpdatePWM(drive_pins_t ioctrl, uint8_t pwm, drivecfg_t cfg)
{
	uint8_t dir = (pwm & 0x80);
	uint8_t spwm = (pwm & 0x7F);
	
	if(cfg.drvDir) dir = !dir;
	
	if(spwm == 0)	{ // is speed is zerro
		DriveRst(ioctrl);
		DriveWind(ioctrl);
		return;
	}
	*ioctrl.MRSTPORT |= (1<<ioctrl.MRST);	
	*ioctrl.APORT |= (1<<ioctrl.AHI);
	*ioctrl.BPORT |= (1<<ioctrl.BHI);
	
	if(dir) {
		if(spwm > cfg.maxPWM_CCW)
			spwm = cfg.maxPWM_CCW;
		
		*ioctrl.TCCRA_B &= ~(1<<COM1A1);
		*ioctrl.TCNT_A = 0;
		*ioctrl.TCNT_B = 0;
		*ioctrl.ICR_A = 0x80;
		*ioctrl.OCRA_A = spwm;
		*ioctrl.TCCRA_A |= (1<<COM1A1);
	} else {
		if(spwm > cfg.maxPWM_CW)
			spwm = cfg.maxPWM_CW;		
		
		*ioctrl.TCCRA_A &= ~(1<<COM1A1);
		*ioctrl.TCNT_A = 0;
		*ioctrl.TCNT_B = 0;
		*ioctrl.ICR_B = 0x80;
		*ioctrl.OCRA_B = spwm;
		*ioctrl.TCCRA_B |= (1<<COM1A1);
	}
}
