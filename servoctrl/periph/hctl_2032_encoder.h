/*
 * hctl_2032_encoder.h
 *
 * Created: 03.04.13 22:30:08
 *  Author: ЖЕНЯ
 */ 

#ifndef HCTL_2032_ENCODER_H_
#define HCTL_2032_ENCODER_H_

#include <avr/io.h>
#include <util/delay.h>

#define HCTL_DATA_PORT		PINA
#define HCTL_CLK_PORT		PORTE
#define HCTL_CFG0_PORT		PORTC
#define HCTL_CFG1_PORT		PORTK

#define HCTL_DATA_DDR		DDRA
#define HCTL_CLK_DDR		DDRE
#define HCTL_CFG0_DDR		DDRC
#define HCTL_CFG1_DDR		DDRK

#define HCTL_CLK_PIN	PINA7		//PE7
#define HCTL_SEL1_PIN		PINA1		//PC1
#define HCTL_SEL2_PIN		PINA2		//PC2
#define HCTL_X_Y_PIN		PINA3		//PC3
#define HCTL_OEN_PIN		PINA4		//PC4
#define HCTL_EN1_PIN		PINA5		//PC5
#define HCTL_EN2_PIN		PINA6		//PC6

#define HCTL_RSTNX_PIN		PINA7		//PK7
#define HCTL_U_DX_PIN		PINA6		//PK6
#define HCTL_CNTDECX_PIN	PINA5		//PK5
#define HCTL_CNTCASX_PIN	PINA4		//PK4
#define HCTL_RSTNY_PIN		PINA3		//PK3
#define HCTL_U_DY_PIN		PINA2		//PK2
#define HCTL_CNTDECY_PIN	PINA1		//PK1
#define HCTL_CNTCASY_PIN	PINA0		//PK0


void InitHCTL();
void ResetHCTL(int rstMode);
uint32_t readHCTL(uint8_t axis);
void switchHCTLMode(uint8_t countMode);
void UpdateEncoderValue();

//void parceEncoderValue(uint32_t CurValue, uint8_t direction, uint32_t *result);

#endif /* HCTL_2032_ENCODER_H_ */