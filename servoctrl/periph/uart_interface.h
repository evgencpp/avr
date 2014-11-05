#ifndef __UART_INTERFACE_H__
#define __UART_INTERFACE_H__

#define USART_BAUDRATE		115200
#define BAUD_PRESCALE  (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

void UartInt_init(void);
void UartInt_txByte();

#endif
