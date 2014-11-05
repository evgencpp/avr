/*
 * drive.h
 *
 * Created: 04.04.13 0:17:12
 *  Author: ЖЕНЯ
 */ 
#ifndef DRIVE_H_
#define DRIVE_H_
#include <avr/io.h>

#include "config.h"

#define MAX_SPEED		0x7F


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


/*
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT)) 

//pins definition
#define M0AHI	PINL4		// OCR5B
#define M0ALO	PINL3		// OCR5A
#define M0ADDR	DDRL		// DDR
#define M0APORT	PORTL		// PORT

#define M0BHI	PINB6		// OCR1B
#define M0BLO	PINB5		// OCR1A
#define M0BDDR	DDRB		// DDR
#define M0BPORT	PORTB		// PORT

#define M0RST		PINL5
#define M0RSTDDR	DDRL
#define M0RSTPORT	PORTL

// 2st drive
#define M1AHI	PINE4		// OCR3B
#define M1ALO	PINE3		// OCR3A
#define M1ADDR	DDRE		// DDR
#define M1APORT	PORTE		// PORT

#define M1BHI	PINH4		// OCR4B
#define M1BLO	PINH3		// OCR4A
#define M1BDDR	DDRH		// DDR
#define M1BPORT	PORTH		// PORT

#define M1RST	PINH5
#define M1RSTDDR	DDRH
#define M1RSTPORT	PORTH

#define DRIVE0	0x1
#define DRIVE1	0x2
#define ALL_DRIVE	0x3

void InitDrive(uint8_t idx);
void UpdateDrive(uint8_t idx, uint8_t value);

void DriveRst(uint8_t idx);
void DriveWind(uint8_t idx);
*/
#endif /* DRIVE_H_ */