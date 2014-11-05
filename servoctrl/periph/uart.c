/*
 * uart.c
 *
 * Created: 17.03.13 14:18:53
 *  Author: ЖЕНЯ
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/iom2560.h>

#include "uart.h"
#include "fifo.h"

//FIFO( 64 ) uart0_rx_fifo;
extern FIFO( 64 ) uart0_rx_fifo;


void UART0Init (uint16_t ubrr)
{
	// Set baud rate
	/*UBRR0H = ubrr>>8;
	UBRR0L = ubrr;*/
	UBRR0 = ubrr; 
	
	/*// Set baud rate
	UBRR0H = ubrr>>8;
	UBRR0L = ubrr;
	
	// Enable receiver and transmitter
	UCSR0A = (0<<U2X0);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	
	// Set frame format: 8 bit, no parity, 1 stop bit,
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);*/
	
	
	
	UCSR0A = 0x0;
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);                 // Turn on the transmission and reception circuitry
	UCSR0C = (0 << UCSZ01) | (3 << UCSZ00);             // Stop Bits =1  8-bit
	
	
	
}

void ResetUART0 ( void )
{
	;
}

void PutCharUART0( unsigned char Data )
{
	/*uint8_t Index;

	// Wait for an empty space in the Tx FIFO

	while ( UART0.TxCount == TXSIZE_UART0 ){
		;
	}

	if ( ( UART0.TxCount == 0 ) && ( UCSR0A & (1<<UDRE0) ) ) {
		// Send character directly
		UDR0 = Data;
	}
	else {
		// Enter critical section
		cli();

		// Store in buffer
		Index = UART0.TxLast;
		UART0.pTxBuffer[ Index ] = Data;

		// Modify buffer pointers
		UART0.TxLast = (unsigned char)((UART0.TxLast + 1) % TXSIZE_UART0);
		UART0.TxCount++;

		// Enable transmit interrupt
		UCSR0B |= (1<<TXCIE0);

		// Exit critical section
		sei();
	}*/
}

void PutStringUART0(uint8_t *pString )
{
	while( *pString ){
		PutCharUART0( (uint8_t)(*pString++));
	}       
	return;
}

void PutDataUART0( uint8_t *pData, uint8_t Length )
{
	while( Length-- ){
		PutCharUART0( *pData++ ); 
	}
	return;
}

signed char GetDataUART0(uint8_t *pData, uint8_t Length)
{
	/*signed char Status;
	unsigned char *iData = pData;
	// Wait for received data
	do {
		// Wait for received character
		do {
			Status = GetCharUART0( iData );
			if( Status == STATUS_ERROR ){       
				// Problem detected
				return STATUS_ERROR;
			}
		} while( Status != STATUS_OK );
		iData++;
	} while( --Length );

	return STATUS_OK;*/
	return 1;
}


//USART0_TXC_vect
SIGNAL(USART0_TX_vect) //ïåðåäà÷à
{
	/*unsigned char Status;
	unsigned char Index;

	//Read the line status code. It is necessary to clear interrupt.
	Status =  UCSR0A & (1<<UDRE0) ;

	while( Status && UART0.TxCount )	{
		//Send a character
		Index = UART0.TxFirst;
		UDR0 = UART0.pTxBuffer[ Index ];

		//Modify buffer pointers
		UART0.TxFirst = (unsigned char)((UART0.TxFirst + 1) % TXSIZE_UART0);
		UART0.TxCount--;        

		Status =  UCSR0A & (1<<UDRE0) ;
	}

	//More data to send?
	if( !UART0.TxCount )	{
		//Disable UART data register empty interrupt
		UCSR0B &= ~(1<<TXCIE0);
	}
	*/
}

SIGNAL(USART0_RX_vect) //ïðè¸ì
{
	uint8_t rxbyte = UDR0;
	/*
	if(!FIFO_IS_FULL( uart0_rx_fifo )) {
		FIFO_PUSH( uart0_rx_fifo, rxbyte );
	}
	*/
}

