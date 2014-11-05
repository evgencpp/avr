/*
 * proc_events.h
 *
 * Created: 17.09.2013 3:13:40
 *  Author: evgen
 */ 


#ifndef PROC_EVENTS_H_
#define PROC_EVENTS_H_

#include "protconst.h"
#include "uart_interface.h"
#include "hdlc.h"
#include "hctl_2032_encoder.h"
#include "uimanager.h"
#include "drive.h"
#include "config.h"

#define TestBit(x,y) (x&(1<<y))

#define PUBLISH_EVENT(event) Exec_writeEventFifo(event)
#define PUBLISH_FAST_EVENT(event) fastEventBitmask |= event

void Exec_run(void);
void FlashLeds(void);
void InitBaseIO(void);

/* Extern Data */
extern unsigned char fastEventBitmask;
extern unsigned char Exec_eventFifo[];
extern volatile unsigned char Exec_eventFifoHead;
extern volatile unsigned char Exec_eventFifoTail;
extern uint8_t encodersMode, lastecodersMode;
extern uint32_t tenc0, tenc1;
extern uint32_t enc0, enc1;
extern int32_t speed0, speed1;
void Exec_writeEventFifo(unsigned char event);
//extern uint16_t calEvTime;		// time for calibration
//extern uint8_t dvg0_cal_val, dvg1_cal_val, enc_calibr, enc_calibr_idx;
//extern DriveConst_t dvg0, dvg1;
// drivers io perith
#define DRV_CNT  (sizeof(drv_pins)/sizeof(drv_pins[0]))
extern drive_pins_t drv_pins[2];
extern drivecfg_t drvCfg[DRV_CNT];

extern volatile uint16_t tcurSns0, tcurSns1;
//PerifClk
extern uint8_t pcDiv, pcOCR;

// evaluate encoder
extern uint8_t evalEnc, evalDvgVal, evalMode, evalRdy;
extern uint16_t evalPointsCnt, evalPointsInd;
extern uint8_t *evalMemory;
extern uint32_t tEvalEnc, lEvalEnc;
extern uint8_t txmsg[UI_MGR_TX_FIFO_SIZE];

#endif /* PROC_EVENTS_H_ */