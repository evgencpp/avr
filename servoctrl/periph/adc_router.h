/*
 * adc_router.h
 *
 * Created: 03.04.13 22:30:08
 *  Author: ЖЕНЯ
 */ 

#ifndef ADC_ROUTER_H_
#define ADC_ROUTER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/iom2560.h>
#define ADMUX_CH0 (1<<REFS0)
#define ADMUX_CH2 ((1<<REFS0) | (1<< MUX1))


#define StartConvAdc()  (ADCSRA |= (1<<ADSC))

void InitADC();



#endif /* HCTL_2032_ENCODER_H_ */