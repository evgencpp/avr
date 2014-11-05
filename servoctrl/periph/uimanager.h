/*
 * uimanager.h
 *
 * Created: 04.04.13 0:17:12
 *  Author: ЖЕНЯ
 */ 


#ifndef _UIMANAGER_H_
#define _UIMANAGER_H_

#include "events.h"
#define BIT_XOR(a,b) ((a) ^= (1<<(b)))
// buffer definitions
#define UI_MGR_RX_FIFO_SIZE 128
#define UI_MGR_RX_FIFO_MASK UI_MGR_RX_FIFO_SIZE-1

#define UI_MGR_TX_FIFO_SIZE 128
#define UI_MGR_TX_FIFO_MASK UI_MGR_TX_FIFO_SIZE-1

/* External Variable Reference */
extern unsigned char UIMgr_rxFifo[];
extern volatile unsigned char UIMgr_rxFifoHead;
extern volatile unsigned char UIMgr_rxFifoTail;

extern unsigned char UIMgr_txFifo[UI_MGR_TX_FIFO_SIZE];
extern volatile unsigned char UIMgr_txFifoHead;
extern volatile unsigned char UIMgr_txFifoTail;

/*  Definitions */
#define IS_DATA_IN_TX_FIFO() (!(UIMgr_txFifoHead == UIMgr_txFifoTail))
#define IS_DATA_IN_RX_FIFO() (!(UIMgr_rxFifoHead == UIMgr_rxFifoTail))


void UIMgr_dispatchEvent(uint8_t event);
void UIMgr_transmitPendingData(void);
void UIMgr_processReceivedData(void);
void UIMgr_flushTxBuffer(void);
void UIMgr_writeTxFifo(unsigned char data);
unsigned char UIMgr_readTxFifo(void);
void onRxFrame(const uint8_t *buffer, uint16_t bytes_received);


#endif /* _UIMANAGER_H_ */