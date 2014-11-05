#ifndef SHORT_APP_H_
#define SHORT_APP_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


// events
#define EV_SERIAL0_RECEIVED			0x01
#define EV_SERIAL0_PENDING_TX	0x90



#define EVENT_FIFO_SIZE	16
#define EVENT_FIFO_MASK EVENT_FIFO_SIZE-1


extern volatile uint8_t event_fifo_h;
extern volatile uint8_t event_fifo_t;
extern uint8_t event_fifo[EVENT_FIFO_SIZE];


#endif