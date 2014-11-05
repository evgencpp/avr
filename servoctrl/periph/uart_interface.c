#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/iom2560.h>

#include "uart_interface.h"
#include "uimanager.h"
#include "proc_events.h"


void UartInt_init(void)
{
	// Set baud rate
	//UBRR0 = 8;
	//BAUD_PRESCALE; 
	UBRR0 = 16;
	
	UCSR0A = (1<<U2X0);
	
	// Turn on the transmission and reception circuitry
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	// Stop Bits =1  8-bit
	UCSR0C = (0 << UCSZ01) | (3 << UCSZ00);
}

void UartInt_txByte()
{
	if(IS_DATA_IN_TX_FIFO()){
		UDR0 = UIMgr_readTxFifo();
		UCSR0B |= (1 << TXCIE0);
	}
}

SIGNAL(USART0_TX_vect)
{
	if(IS_DATA_IN_TX_FIFO()){
		UDR0 = UIMgr_readTxFifo();
	} else {
		UCSR0B &= ~(1 << TXCIE0);
	}
}

SIGNAL(USART0_RX_vect)
{
	unsigned char tmpHead;
    /* read the data byte, put it in the serial queue, and
    post the event */
 
    UIMgr_rxFifo[UIMgr_rxFifoHead] = UDR0;

    /* now move the head up */
    tmpHead = (UIMgr_rxFifoHead + 1) & (UI_MGR_RX_FIFO_MASK);
    UIMgr_rxFifoHead = tmpHead;
    
    /* write the serial received event to the event fifo */
    Exec_eventFifo[Exec_eventFifoHead] = EV_SERIAL_DATA_RECEIVED;

    /* now move the head up */
    tmpHead = (Exec_eventFifoHead + 1) & (EXEC_EVENT_FIFO_MASK);
    Exec_eventFifoHead = tmpHead;
	PORTD ^=(1<<7);
}
