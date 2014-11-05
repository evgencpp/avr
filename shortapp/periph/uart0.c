#include "uart0.h"
#include "..//ShortApp.h"
// UART 0 definitions
#define BAUD		115200		// 115200 baurate
#define USE_2X		1			// uart 2x
#define BAUD_TOL	3			// 3% errors (2.1)

#include <avr/interrupt.h>
#include <util/setbaud.h>

// local variables
/* Fifo variable Reference */
uint8_t rxFifo0[TX0_FIFO_SIZE];
volatile uint16_t rx_fifo0_h;	// Head
volatile uint16_t rx_fifo0_t;	// Tail

uint8_t txFifo0[TX0_FIFO_SIZE];
volatile uint16_t tx_fifo0_h;	// Head
volatile uint16_t tx_fifo0_t;	// Tail

//external variables

/************************************************************************/
/* Initialize UART module                                               */
/************************************************************************/
void InitUart0()
{
	UBRR0	= UBRR_VALUE;
	UCSR0A	= (1 << U2X0);
	
	// Turn on the transmission and reception circuitry
	UCSR0B	= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	// Stop Bits =1  8-bit
	UCSR0C	= (0 << UCSZ01) | (3 << UCSZ00);				
}

/***********************************************************
	Function Name: UIMgr_readTxFifo
	Function Description: This function is responsible for
	reading a single byte of data from the tx fifo, and
	updating the appropriate pointers.
	Inputs:  none 
	Outputs: unsigned char-the data read
***********************************************************/	
uint8_t readTxFifo0(void)
{
	uint8_t dataByte, tmpTail;
	
	/* just return the current tail from the tx fifo */
	cli();
	dataByte = txFifo0[tx_fifo0_t];	
	tmpTail = (tx_fifo0_t + 1) & (TX0_FIFO_MASK);
	tx_fifo0_t = tmpTail;
	sei();
	return(dataByte);
}

/*
void Uart0_txByte()
{
	// if data present in fifo return it
	if(IS_DATA_IN_TX_FIFO())	{
		UDR0 = readTxFifo0();
		UCSR0B |= (1 << TXCIE0);
	}
}
*/
/************************************************************************/
/* TX Buffer empty interrupt function                                   */
/************************************************************************/
SIGNAL(USART0_TX_vect)
{
	if(IS_DATA_IN_TX_FIFO()){
		UDR0 = readTxFifo0();
	} else {
		UCSR0B &= ~(1 << TXCIE0);
	}
}

/************************************************************************/
/* RX byte interrupt function                                           */
/************************************************************************/
SIGNAL(USART0_RX_vect)
{
	uint8_t tmpHead;
    /* read the data byte, put it in the serial queue, and
    post the event */
	 
    rxFifo0[rx_fifo0_h] = UDR0;

    /* now move the head up */
    tmpHead = (rx_fifo0_h + 1) & (RX0_FIFO_MASK);
    rx_fifo0_h = tmpHead;
    
    /* write the serial received event to the event fifo */
    event_fifo[event_fifo_h] = EV_SERIAL0_RECEIVED;

    /* now move the head up */
    tmpHead = (event_fifo_h + 1) & (EVENT_FIFO_MASK);
    event_fifo_h = tmpHead;
}


/***********************************************************
	Function Name: UIMgr_readRxFifo
	Function Description: This function is responsible for
	reading a single byte of data from the rx fifo, and
	updating the appropriate pointers.
	Inputs:  none 
	Outputs: unsigned char-the data read
***********************************************************/	
uint8_t read_serial0_fifo(void)
{
	uint8_t dataByte, tmpTail;
	
	/* just return the current tail from the rx fifo */
	cli();
	dataByte = rxFifo0[rx_fifo0_t];
	tmpTail = (rx_fifo0_t + 1) & (RX0_FIFO_MASK);
	rx_fifo0_t = tmpTail;
	sei();
	
	return(dataByte);
}

/***********************************************************
	Function Name: UIMgr_transmitPendingData
	Function Description: This function is responsible for
	transmitting a single byte of data if data is waiting
	to be sent.  Otherwise, if nothing is waiting, the
	function just returns.
	Inputs:  none 
	Outputs: none
***********************************************************/
void transmit_pending_data0()
{
	if(IS_DATA_IN_TX_FIFO()){
		UDR0 = readTxFifo0();
		UCSR0B |= (1 << TXCIE0);
	}
}

/***********************************************************
	Function Name: UIMgr_writeTxFifo
	Function Description: This function is responsible for
	writing a single byte to the TxFifo and
	updating the appropriate pointers.
	Inputs:  data - the byte to write to the Fifo 
	Outputs: none
***********************************************************/	
void write_serial0_fifo(uint8_t data)
{
	unsigned char tmpHead;
	cli();
	txFifo0[tx_fifo0_h] = data;

    /* now move the head up */
    tmpHead = (tx_fifo0_h + 1) & (TX0_FIFO_MASK);
    tx_fifo0_h = tmpHead;
	sei();
}
