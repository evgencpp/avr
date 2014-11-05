/*
 * hctl_2032_encoder.c
 *
 * Created: 03.04.13 22:30:27
 *  Author: ЖЕНЯ
 */ 

#include "hctl_2032_encoder.h"
#include "config.h"
#include "proc_events.h"

// last encoder values
static uint32_t lenc0, lenc1;
uint8_t lenc_dir0, lenc_dir1;
void InitHCTLIO()
{	
	// init DDR pins
	HCTL_CLK_DDR |= (1<<HCTL_CLK_PIN);
	
	HCTL_CFG0_DDR |= ((1<<HCTL_SEL1_PIN) | (1<<HCTL_SEL2_PIN) |
					(1<<HCTL_X_Y_PIN) | (1<<HCTL_OEN_PIN));
	
	HCTL_CFG1_DDR |= ((1<<HCTL_RSTNX_PIN) | (1<<HCTL_RSTNY_PIN));
	
	HCTL_CFG0_PORT |= (1<<HCTL_SEL2_PIN);
}

void InitHCTL()
{
	lenc0 = 0;
	lenc1 = 0;
	
	InitHCTLIO();
	switchHCTLMode(encodersMode);	/*4, 2, 1*/
	ResetHCTL(0x3);
}

void ResetHCTL(int rstMode)
{
	switch(rstMode){
		case 0x1:
			enc0 = 0x0;
			lenc0 = 0;
			HCTL_CFG1_PORT &= ~(1<<HCTL_RSTNX_PIN);
			_delay_us(50);
			HCTL_CFG1_PORT |= (1<<HCTL_RSTNX_PIN);
			break;
		case 0x2:
			enc1 = 0x0;
			lenc1 = 0;
			HCTL_CFG1_PORT &= ~(1<<HCTL_RSTNY_PIN);
			_delay_us(50);
			HCTL_CFG1_PORT |= (1<<HCTL_RSTNY_PIN);
			break;
		case 0x3:
			lenc0 = 0;
			enc0 = 0x0;
			HCTL_CFG1_PORT &= ~(1<<HCTL_RSTNX_PIN);
			_delay_us(50);
			HCTL_CFG1_PORT |= (1<<HCTL_RSTNX_PIN);
			
			enc1 = 0x0;
			HCTL_CFG1_PORT &= ~(1<<HCTL_RSTNY_PIN);
			_delay_us(50);
			HCTL_CFG1_PORT |= (1<<HCTL_RSTNY_PIN);
			
			break;
		default: break;
	}
	
}

uint32_t readHCTL(uint8_t axis) {
//~64us
	//static uint8_t read_data[4];
	static uint32_t counter = 0;
	//lenc_dir0, lenc_dir1
	// select x or y counter
	if (axis == 1){	// read Y counter
		lenc_dir1 = (HCTL_CFG1_PORT & (1<<HCTL_U_DY_PIN));
		HCTL_CFG0_PORT |= (1<<HCTL_X_Y_PIN);	// set to hight
	}
	else {			// read X counter
		lenc_dir0 = (HCTL_CFG1_PORT & (1<<HCTL_U_DX_PIN));
		HCTL_CFG0_PORT &=~(1<<HCTL_X_Y_PIN);	// set to low
	}

	// step 1: read MSB
	HCTL_CFG0_PORT &= ~(1<<HCTL_OEN_PIN);
	asm volatile("nop\n\t");
	asm volatile("nop\n\t");
	//read_data[0] = HCTL_DATA_PORT;
	//counter = ((uint32_t)read_data[0] << 24);
	counter = ((uint32_t)HCTL_DATA_PORT << 24);

	// step 2: read 2nd byte
	HCTL_CFG0_PORT |= (1<<HCTL_SEL1_PIN);
	asm volatile("nop\n\t");
	asm volatile("nop\n\t");
	//read_data[1] = HCTL_DATA_PORT;
	//counter = counter | ((uint32_t)read_data[1] << 16);
	counter = counter | ((uint32_t)HCTL_DATA_PORT << 16);
	
	// step 3: read 3rd byte
	HCTL_CFG0_PORT &= ~((1<<HCTL_SEL1_PIN)|(1<<HCTL_SEL2_PIN));
	asm volatile("nop\n\t");
	asm volatile("nop\n\t");
	//read_data[2] = HCTL_DATA_PORT;
	//counter = counter | (read_data[2] << 8);
	counter = counter | (HCTL_DATA_PORT << 8);

	// step 4: read LSB
	HCTL_CFG0_PORT |= (1<<HCTL_SEL1_PIN);
	asm volatile("nop\n\t");
	asm volatile("nop\n\t");
	//read_data[3] = HCTL_DATA_PORT;
	//counter = counter | read_data[3];
	counter = counter | HCTL_DATA_PORT;

	// step 5: complete inhibit logic reset
	HCTL_CFG0_PORT |= (1<<HCTL_OEN_PIN);
	HCTL_CFG0_PORT |= (1<<HCTL_SEL2_PIN);
	HCTL_CFG0_PORT &= ~(1<<HCTL_SEL1_PIN);
	return counter;
}
	
void switchHCTLMode( uint8_t countMode )
{
	// Count Mode Illegal Mode EN1 LOW  EN2 LOW
	// Count Mode   4X         EN1 HIGH EN2 LOW
	// Count Mode   2X         EN1 LOW  EN2 HIGH
	// Count Mode   1X         EN1 HIGH EN2 HIGH
	switch(countMode)	{
		case 1: // 1X Count Mode
			//digitalWrite(uno_Counter_PIN_EN1, HIGH);
			//digitalWrite(uno_Counter_PIN_EN2, HIGH);
			HCTL_CFG0_PORT |= ((1<<HCTL_EN1_PIN) | (1<<HCTL_EN2_PIN));
			break;
		case 2: // 2X Count Mode
			//digitalWrite(uno_Counter_PIN_EN1, LOW);
			//digitalWrite(uno_Counter_PIN_EN2, HIGH);
			HCTL_CFG0_PORT &= ~(1<<HCTL_EN1_PIN);
			HCTL_CFG0_PORT |= (1<<HCTL_EN2_PIN);
			break;
		case 4: // 4X Count Mode is the default
		default:
			//digitalWrite(uno_Counter_PIN_EN1, HIGH);
			//digitalWrite(uno_Counter_PIN_EN2, LOW);
			HCTL_CFG0_PORT |= (1<<HCTL_EN1_PIN);
			HCTL_CFG0_PORT &= ~(1<<HCTL_EN2_PIN);
			break;
	}
	_delay_us(2);
}

//uint32_t enc0, enc1;
// Process encoder values
void UpdateEncoderValue()
{
	if(lenc0 != tenc0)	{
		if(lenc_dir0){
			speed0 = lenc0 - tenc0;
			enc0 -= (drvCfg[0].encDir)?-speed0:speed0;
		} else {
			speed0 = tenc0 - lenc0;
			enc0 += (drvCfg[0].encDir)?-speed0:speed0;
		}
		lenc0 = tenc0;
	}
	if(lenc1 != tenc1)	{
		if(lenc_dir1){
			speed1 = lenc1 - tenc1;
			enc1 -= (drvCfg[1].encDir)?-speed1:speed1;
		} else {
			speed1 = tenc1 - lenc1;
			enc1 += (drvCfg[1].encDir)?-speed1:speed1;
		}
		lenc1 = tenc1;
	}
	
	/*
	lenc_dir1
	lenc_dir0
	// proc first encoder
	if(lenc0 != tenc0){
		if(drvCfg[0].encDir){
			if(tenc0 > lenc0){
				speed0 = tenc0 - lenc0;
				if(speed0 != 0) enc0 -= speed0;
			} else {
				speed0 = lenc0 - tenc0;
				if(speed0 != 0) enc0 += speed0;
			}
		} else {
			if(tenc0 > lenc0){
				speed0 = tenc0 - lenc0;
				if(speed0 != 0) enc0 += speed0;
			} else {
				speed0 = lenc0 - tenc0;
				if(speed0 != 0) enc0 -= speed0;
			}
		}
		lenc0 = tenc0;
	}
	
	// proc second encoder
	if(lenc1 != tenc1){
		if(drvCfg[1].encDir){
			if(tenc1 > lenc1){
				speed1 = tenc1 - lenc1;
				if(speed1 != 0) enc1 -= speed1;
			} else {
				speed1 = lenc1 - tenc1;
				if(speed1 != 0) enc1 += speed1;
			}
		} else {
			if(tenc1 > lenc1){
				speed1 = tenc1 - lenc1;
				if(speed1 != 0) enc1 += speed1;
			} else {
				speed1 = lenc1 - tenc1;
				if(speed1 != 0) enc1 -= speed1;
			}
		}
		lenc1 = tenc1;
	}
	*/
}