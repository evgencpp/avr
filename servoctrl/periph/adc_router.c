/*
 * adc_router.c
 *
 * Created: 03.04.13 22:30:27
 *  Author: ЖЕНЯ
 */ 


#include "adc_router.h"
#include "proc_events.h"

void InitADC()
{
	// AVCC with external capacitor at AREF pin
	ADMUX =	(1<<REFS0);
	
	/*
		ADATE - continues mode
		ADEN - enable ADC
		ADSC - start conversion
	*/
	
	//Enable the ADC and set PS.
	ADCSRA = (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0) | (1<<ADATE) | (1<<ADEN) | (1<<ADIE); 
	//ADCSRA = (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0) | (1<<ADEN) | (1<<ADIE); 
	ADCSRB = 0;
}

// PF0 (ADC0)
// PF2 (ADC2)
SIGNAL(ADC_vect)
{
	if(ADMUX == ADMUX_CH0){
		tcurSns1 = ADCW;
		ADMUX = ADMUX_CH2;
	}
	else {
		tcurSns0 = ADCW;
		ADMUX = ADMUX_CH0;
	}
}
