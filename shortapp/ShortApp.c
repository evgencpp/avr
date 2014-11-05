/*
 * ShortApp.c
 *
 * Created: 1/7/2014 23:25:32
 *  Author: Evgeny
 */ 
#include "ShortApp.h"
#include "periph/uart0.h"
#include "periph/hdlc.h"
#include "periph/prot.h"
#include "periph/drive.h"

/*  Local Variables */
// event fifo
volatile uint8_t event_fifo_h;	// Fifo Head
volatile uint8_t event_fifo_t;	// Fifo Tail
#define IS_DATA_IN_EVENT_FIFO() (!(event_fifo_h == event_fifo_t))
uint8_t event_fifo[EVENT_FIFO_SIZE];

static int8_t Version[] = "v1.01 20.09.2013\0";
uint8_t txmsg[TX0_FIFO_SIZE];
uint8_t *txmsgptr;


static uint8_t err_rx0_len, err_rx0_head, err_rx0_good, err_tx0_good;
/************************************************************************/
/* Read evgent fifo                                                     */
/************************************************************************/
static uint8_t read_event_fifo(void)
{
	uint8_t dataByte, tmpTail;
	
	cli();
	/* just return the current tail from the tx fifo */
	dataByte = event_fifo[event_fifo_t];
	tmpTail = (event_fifo_t + 1) & (EVENT_FIFO_MASK);
	event_fifo_t = tmpTail;
	sei();
	
	return dataByte;
}

/***********************************************************
	Function Name: Exec_writeEventFifo
	Function Description: This function is responsible for
	writing a single event to the event fifo and
	updating the appropriate pointers.
	Inputs:  data - the byte to write to the Fifo 
	Outputs: none
***********************************************************/	
void push_event(uint8_t event)
{
	uint8_t tmpHead;

	cli();
	event_fifo[event_fifo_h] = event;

    /* now move the head up */
    tmpHead = (event_fifo_h + 1) & (EVENT_FIFO_MASK);
    event_fifo_h = tmpHead;
	sei();
}

/************************************************************************/
/* Process input uart messages                                          */
/************************************************************************/
void OnSerial0Frame(const uint8_t *buffer, uint16_t bytes_received)
{
	static int8_t *pData;
	
	if(buffer[1] == bytes_received){
		switch(buffer[1]){
			case CMD_PING:
				err_rx0_good++;
				txmsg[0] = PING_ACK_CMD;
				txmsg[1] = PING_ACK_CMD_LEN;
				txmsg[2] = buffer[2] + 1;
				err_tx0_good++;
				hdlc_tx_frame(txmsg, PING_ACK_CMD_LEN);
				push_event(EV_SERIAL0_PENDING_TX);
			break;
			case CMD_VER:
				err_rx0_good++;
				pData = Version;
				txmsg[0] = VERSION_ACK_CMD;
				txmsg[1] = VERSION_ACK_CMD_LEN;
				txmsg[2] = buffer[2] + 1;
				txmsgptr = txmsg + 3;
				while(*pData != 0) {
					*txmsgptr++ = *pData++;
				}
				err_tx0_good++;
				hdlc_tx_frame(txmsg, VERSION_ACK_CMD_LEN);
				push_event(EV_SERIAL0_PENDING_TX);
			break;
			case CMD_STAT:
				err_rx0_good++;
				txmsg[0] = STAT_ACK_CMD;
				txmsg[1] = STAT_ACK_CMD_LEN;
				txmsg[2] = buffer[2] + 1;
				txmsg[3] = err_rx0_head;
				txmsg[4] = err_rx0_len;
				txmsg[5] = err_rx0_good;
				txmsg[6] = err_tx0_good++;
				hdlc_tx_frame(txmsg, STAT_ACK_CMD_LEN);
				push_event(EV_SERIAL0_PENDING_TX);
			break;
			default:
				err_rx0_head++;
				break;
		}
	} else {
		err_rx0_len++;
	}
}

drive_pins_t drv_pins[2] = {
	{PINL4, PINL3, &DDRL, &PORTL,
	PINB6, PINB5, &DDRB, &PORTB,
	PINL5, &DDRL, &PORTL,
	&TCNT5, &TCCR5A, &TCCR5B, &ICR5, &OCR5A,
	&TCNT1, &TCCR1A, &TCCR1B, &ICR1, &OCR1A
	},
	{PINE4, PINE3, &DDRE, &PORTE,
	PINH4, PINH3, &DDRH, &PORTH,
	PINH5, &DDRH, &PORTH,
	&TCNT3, &TCCR3A, &TCCR3B, &ICR3, &OCR3A,
	&TCNT4, &TCCR4A, &TCCR4B, &ICR4, &OCR4A}
};
drivecfg_t curcfg[2];

#define DRV_CNT  (sizeof(drv_pins)/sizeof(drv_pins[0]))
/************************************************************************/
/* Main function                                                        */
/************************************************************************/
static uint8_t gen_event;
volatile uint8_t i;
int main(void)
{
	err_rx0_len = 0;
	event_fifo_h = 0;
	event_fifo_t = 0;	
	err_rx0_head = 0;
	txmsgptr = txmsg;
	DDRA = 0; DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0; 
	DDRJ = 0; DDRH = 0; DDRJ = 0; DDRK = 0; DDRL = 0;
	
	for(i = 0; i < DRV_CNT; i++){
		InitDrive(drv_pins[i]);
	}
	
	InitUart0();
	hdlc_init(write_serial0_fifo, OnSerial0Frame);
		
	_delay_ms(1);	// wait 1 ms
	sei();			// global enable interrupts

	// main loop
    for(;;)	{
		if (IS_DATA_IN_EVENT_FIFO())	{
			gen_event = read_event_fifo();	
			switch (gen_event){
				case EV_SERIAL0_RECEIVED:
					// read all rx fifo data
					while(IS_DATA_IN_RX_FIFO())
						hdlc_on_rx_byte(read_serial0_fifo());
					break;
				case EV_SERIAL0_PENDING_TX:
					transmit_pending_data0();
			}
		} else {
			
		}
    }
	
}


