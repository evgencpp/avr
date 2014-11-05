#ifndef _UART0_H_
#define _UART0_H_

#include <avr/io.h>
#include <avr/iom2560.h>


// Rx fifo size
#define RX0_FIFO_SIZE 128
#define RX0_FIFO_MASK RX0_FIFO_SIZE-1

// Tx fifo size
#define TX0_FIFO_SIZE 128
#define TX0_FIFO_MASK TX0_FIFO_SIZE-1

/* External Variable Reference */
extern uint8_t rxFifo0[TX0_FIFO_SIZE];
extern volatile uint16_t rx_fifo0_h;	// Head
extern volatile uint16_t rx_fifo0_t;	// Tail
extern uint8_t txFifo[TX0_FIFO_SIZE];
extern volatile uint16_t tx_fifo0_h;	// Head
extern volatile uint16_t tx_fifo0_t;	// Tail

#define IS_DATA_IN_TX_FIFO() (!(tx_fifo0_h == tx_fifo0_t))
#define IS_DATA_IN_RX_FIFO() (!(rx_fifo0_t == rx_fifo0_t))

void InitUart0();
//uint8_t readTxFifo0(void);
uint8_t read_serial0_fifo(void);
void transmit_pending_data0(void);
void write_serial0_fifo(uint8_t data);


#endif
