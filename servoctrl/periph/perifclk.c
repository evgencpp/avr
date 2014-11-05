/*
 * perifclk.c
 *
 * Created: 17.03.13 14:18:53
 *  Author: ЖЕНЯ
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

#include "perifclk.h"
#include "events.h"
#include "proc_events.h"

extern uint8_t pcDiv, pcOCR;

SIGNAL(TIMER0_COMPA_vect)
{
	fastEventBitmask |= FEV_SYS_TIMER_COMPLETE;
	
}
//---------------------------------------------------------------------------
void InitPerifClk(void)
{	
	TCCR0B |= (1 << WGM01);
	// set up timer with prescaler = 64 and CTC mode
	TCCR0B |= (1 << CS01)|(1 << CS00);

	// initialize counter
	TCNT0 = 0;

	TIMSK0 |= (1<<OCIE0A);
	
	// initialize compare value
	OCR0A =		249;

	pcDiv = (1 << CS01)|(1 << CS00);
	pcOCR = 249;

}
//---------------------------------------------------------------------------
void SetPerifClk(uint8_t div, uint8_t OCR)
{
	pcDiv = div;
	pcOCR = OCR;
}
//---------------------------------------------------------------------------
void StartPerifClk(void)
{
	TCCR0B |= (1 << WGM01);

	// set up timer with prescaler = 64 and CTC mode
	TCCR0B = pcDiv;

	// initialize counter
	TCNT0 = 0;

	TIMSK0 |= (1<<OCIE0A);

	// initialize compare value
	OCR0A =		pcOCR;
}
//---------------------------------------------------------------------------
void StopPerifClk(void)
{
	TCCR0A = 0;
	TCCR0B = 0;
	TCNT0 = 0;
	TIMSK0 = 0;
	OCR0A =	0;
}
//---------------------------------------------------------------------------


