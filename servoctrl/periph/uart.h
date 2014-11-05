/*
 * uart.h
 *
 * Created: 17.03.13 14:19:10
 *  Author: ЖЕНЯ
 */ 


#ifndef UART_H_
#define UART_H_

#define TXSIZE_UART0        0xFF        /* TX buffer size */
#define RXSIZE_UART0        0xFF        /* RX buffer size */

void UART0Init (uint16_t ubrr);
void          ResetUART0      ( void );
void          PutCharUART0    ( uint8_t );
void          PutStringUART0  ( uint8_t * );
void          PutDataUART0    ( uint8_t *, uint8_t );
signed char   GetCharUART0    ( uint8_t * );
signed char   GetStringUART0  ( uint8_t * , uint8_t );
signed char   GetDataUART0    ( uint8_t *, uint8_t );
void          SetEOStrUART0   ( uint8_t  );

void          HandlerUART0    ( void );
signed char   GetLenUART0     ( void );
void          PurgeUART0      ( void );
//unsigned char GetUART0Message ( UARTMessage * RxMessag );

void UART_respond (void);

#endif /* UART_H_ */