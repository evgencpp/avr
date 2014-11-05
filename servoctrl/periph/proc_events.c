/*
 * proc_events.c
 *
 * Created: 17.09.2013 3:13:25
 *  Author: evgen
 */ 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#include "proc_events.h"


/* drives configureation pins */

// drivers io perith
drive_pins_t drv_pins[2] = {
	{PINL4, PINL3, &DDRL, &PORTL,
		PINB6, PINB5, &DDRB, &PORTB,
		PINL5, &DDRL, &PORTL,
		&TCNT1, &TCCR1A, &TCCR1B, &ICR1, &OCR1A,
		&TCNT5, &TCCR5A, &TCCR5B, &ICR5, &OCR5A
		
	},
	{PINE4, PINE3, &DDRE, &PORTE,
		PINH4, PINH3, &DDRH, &PORTH,
		PINH5, &DDRH, &PORTH,
		&TCNT4, &TCCR4A, &TCCR4B, &ICR4, &OCR4A,
		&TCNT3, &TCCR3A, &TCCR3B, &ICR3, &OCR3A}
};
// drivel configuration
drivecfg_t drvCfg[DRV_CNT];

/*  Local Variables */
unsigned char Exec_eventFifo[EXEC_EVENT_FIFO_SIZE];
volatile unsigned char Exec_eventFifoHead;
volatile unsigned char Exec_eventFifoTail;

/*  Local Function Definitions */
static unsigned char Exec_readEventFifo(void);

/* This bitmask holds events that need to be processed as fast as possible */
unsigned char fastEventBitmask = 0x00;
uint8_t encodersMode = 4, lastecodersMode;

/*  Definitions */
#define IS_DATA_IN_EVENT_FIFO() (!(Exec_eventFifoHead == Exec_eventFifoTail))
uint32_t tenc0, tenc1;
uint32_t tEvalEnc, lEvalEnc;
uint32_t enc0, enc1;
uint8_t enc_ch=0;
int32_t speed0, speed1;

// current sensor
volatile uint16_t tcurSns0, tcurSns1;

//PerifClk
uint8_t pcDiv, pcOCR;
// evaluate encoder
uint8_t evalEnc, evalDvgVal, evalMode = 0, evalRdy = 0;
uint16_t evalPointsCnt, evalPointsInd;
uint8_t *evalMemory;
uint8_t txmsg[UI_MGR_TX_FIFO_SIZE];

/*******************************************************************************
	Function Name: Exec_run
	Function Description: This function is responsible for
	running the main control loop.  The control loop is 
	based on checking both the fast-event bitmask (for high
    priority events) and the event FIFO to determine if an
    event needs to be handled.  The event is then dispatched
    to the appropriate handler.
	Inputs:  none
	Outputs: none
*******************************************************************************/
//uint16_t calEvTime;
//uint8_t dvg0_cal_val, dvg1_cal_val, enc_calibr, enc_calibr_idx;
//DriveConst_t dvg0, dvg1;

void Exec_run(void)
{
	uint8_t eventGenerated;
	
	for(;;)	{
		if (fastEventBitmask)	{
			if(fastEventBitmask & FEV_SYS_TIMER_COMPLETE){
				if(evalMode == 1){	// evaluate encoder value
					if(evalRdy == 0){	// if not ready	
						tEvalEnc = readHCTL(evalEnc);		//136clk (8.5us)
						//evalPointsCnt, evalPointsInd
						if(evalPointsInd < evalPointsCnt) {
							evalMemory[evalPointsInd] = (uint8_t)(tEvalEnc - lEvalEnc);
							lEvalEnc = tEvalEnc;
							evalPointsInd++;
						} else {
							evalRdy = 1;
							evalPointsInd = 0;
						}
					}
				} else {	// normal mode
					tenc0 = readHCTL(0);		//136clk (8.5us)
					tenc1 = readHCTL(1);		//137clk (8.56us)
				}
				BIT_XOR(PORTD, 6);
				fastEventBitmask &= ~FEV_SYS_TIMER_COMPLETE;
				//if(enc_calibr)calEvTime++;
			}
			/* an event needing fast processing has been received */
			/* a received line needs to be processed...this
			needs to be processed as quickly as possible */
			/*if (fastEventBitmask & FEV_ACQUIRE_LINE_COMPLETE)	{
                
			}
			if (fastEventBitmask & FEV_PROCESS_LINE_COMPLETE)	{
                
			}*/
		}		
		
        if (IS_DATA_IN_EVENT_FIFO())	{
            eventGenerated = Exec_readEventFifo();
			switch(eventGenerated)	{
				case (EV_SERIAL_DATA_RECEIVED):
					UIMgr_dispatchEvent(eventGenerated);
					//FrameMgr_dispatchEvent(eventGenerated);
					break;																
				case (EV_SERIAL_DATA_PENDING_TX):
					UIMgr_dispatchEvent(eventGenerated);
					break;
				default:		
					break;
			}
		}
		
		if(evalMode == 1) {
			if(evalRdy == 1) {	// transmit data
				if(evalPointsInd < evalPointsCnt) {
					if(!TestBit(UCSR0B, TXCIE0)){
						txmsg[0] = EVAL_ENC_DATA_CMD;
						txmsg[1] = EVAL_ENC_DATA_CMD_LEN;
						txmsg[2] = evalPointsInd;	// TODO: insert tx_counter
						txmsg[3] = (uint8_t)((evalPointsInd>>8) & 0xff);
						txmsg[4] = (uint8_t)(evalPointsInd & 0xff);
						txmsg[5] = evalMemory[evalPointsInd];
						hdlc_tx_frame(txmsg, EVAL_ENC_DATA_CMD_LEN);
						PUBLISH_EVENT(EV_SERIAL_DATA_PENDING_TX);
						evalPointsInd++;
					}
				} else {
					ResetHCTL(3);
					UpdatePWM(drv_pins[evalEnc], 0, drvCfg[evalEnc]);
					// finish evaluate
					evalMode = 0;		
					evalRdy = 0;
					evalPointsInd = 0;
				}
			}
		} else {
			UpdateEncoderValue();
		}
		
	}
}

static unsigned char Exec_readEventFifo(void)
{
	unsigned char dataByte, tmpTail;
	
	cli();
	/* just return the current tail from the tx fifo */
	dataByte = Exec_eventFifo[Exec_eventFifoTail];
	tmpTail = (Exec_eventFifoTail+1) & (EXEC_EVENT_FIFO_MASK);
	Exec_eventFifoTail = tmpTail;
	sei();
	
	return(dataByte);
}

void InitBaseIO(void)
{	
	DDRA = 0; DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0; DDRG = 0; DDRH = 0; DDRJ = 0; DDRK = 0; DDRL = 0; 
	PORTA = 0; PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0; PORTG = 0; PORTH = 0; PORTJ = 0; PORTK = 0; PORTL = 0;
	
	// Setup leds 4 5 6 7
	DDRD |=  (1<<PIND4) | (1<<PIND5) |(1<<PIND6) |(1<<PIND7);
	PORTD |=  ((1<<PIND4) | (1<<PIND5) |(1<<PIND6) |(1<<PIND7));	//turn off leds
	
	// clear TC
	TCCR0A = 0; TCCR0B = 0; TIFR0 = 0; TIMSK0 = 0; OCR0A = 0; OCR0B = 0;
	TCCR1A = 0; TCCR1B = 0; TCCR1C = 0; TIFR1 = 0; TIMSK1 = 0; ICR1 = 0; OCR1A = 0; OCR1B = 0; OCR1C = 0;
	TCCR2A = 0; TCCR2B = 0; TIFR2 = 0; TIMSK2 = 0; OCR2A = 0; OCR2B = 0; 
	TCCR3A = 0; TCCR3B = 0; TCCR3C = 0; TIFR3 = 0; TIMSK3 = 0; ICR3 = 0; OCR3A = 0; OCR3B = 0; OCR3C = 0;
	TCCR4A = 0; TCCR4B = 0; TCCR4C = 0; TIFR4 = 0; TIMSK4 = 0; ICR4 = 0; OCR4A = 0; OCR4B = 0; OCR4C = 0;
	TCCR5A = 0; TCCR5B = 0; TCCR5C = 0; TIFR5 = 0; TIMSK5 = 0; ICR5 = 0; OCR5A = 0; OCR5B = 0; OCR5C = 0;	
}

void FlashLeds()
{
	uint8_t i;

	i = 3;
	while(i++ < 7){
		PORTD &=~(1<<i);
		_delay_ms(100);
		PORTD |=(1<<i);
		_delay_ms(100);
	}
	_delay_ms(50);
	i = 8;
	while(i-- > 4){
		PORTD &=~(1<<i);
		_delay_ms(100);
		PORTD |=(1<<i);
		_delay_ms(100);
	}

}

/***********************************************************
	Function Name: Exec_writeEventFifo
	Function Description: This function is responsible for
	writing a single event to the event fifo and
	updating the appropriate pointers.
	Inputs:  data - the byte to write to the Fifo 
	Outputs: none
***********************************************************/	
void Exec_writeEventFifo(unsigned char event)
{
	unsigned char tmpHead;

	cli();
	Exec_eventFifo[Exec_eventFifoHead] = event;

    /* now move the head up */
    tmpHead = (Exec_eventFifoHead + 1) & (EXEC_EVENT_FIFO_MASK);
    Exec_eventFifoHead = tmpHead;
	sei();
}

