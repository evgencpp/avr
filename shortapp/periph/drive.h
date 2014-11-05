#ifndef DRIVE_H_
#define DRIVE_H_

#include "..//ShortApp.h"
#include "config.h"

typedef struct tag_drive_pins{
	// pins pointers
	uint8_t AHI;	// A line
	uint8_t ALO;	
	volatile uint8_t *ADDR;
	volatile uint8_t *APORT;

	uint8_t BHI;	// B line
	uint8_t BLO;	
	volatile uint8_t *BDDR;
	volatile uint8_t *BPORT;

	uint8_t MRST;	// RST
	volatile uint8_t *MRSTDDR;
	volatile uint8_t *MRSTPORT;
	
	//PWM CTC register pointers
	volatile uint16_t *TCNT_A;
	volatile uint8_t *TCCRA_A;
	volatile uint8_t *TCCRB_A;
	volatile uint16_t *ICR_A;
	volatile uint16_t *OCRA_A;
	
	volatile uint16_t *TCNT_B;
	volatile uint8_t *TCCRA_B;
	volatile uint8_t *TCCRB_B;
	volatile uint16_t *ICR_B;
	volatile uint16_t *OCRA_B;		
	uint8_t wind;
}drive_pins_t;

void InitDrive(drive_pins_t ioctrl);
void DriveRst(drive_pins_t ioctrl);
void DriveWind(drive_pins_t ioctrl);
void UpdatePWM(drive_pins_t ioctrl, uint8_t pwm, drivecfg_t cfg);

#endif
