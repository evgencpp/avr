/*
 * pid.c
 *
 * Created: 04.04.13 0:16:58
 *  Author: ЖЕНЯ
 */ 
///#include <stdlib.h>
#include <string.h>
///#include <avr/eeprom.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>


#include "proc_events.h"
#include "uimanager.h"

#include "config.h"
#include "drive.h"
#include "hctl_2032_encoder.h"
#include "perifclk.h"

/*
	drivecfg_t curConfig[CONFIG_DVG_CNT];
*/
/*  Extern Variables */
unsigned char UIMgr_rxFifo[UI_MGR_RX_FIFO_SIZE];
volatile unsigned char UIMgr_rxFifoHead=0;
volatile unsigned char UIMgr_rxFifoTail=0;

unsigned char UIMgr_txFifo[UI_MGR_TX_FIFO_SIZE];
volatile unsigned char UIMgr_txFifoHead=0;
volatile unsigned char UIMgr_txFifoTail=0;

/*  Definitions */
#define IS_DATA_IN_TX_FIFO() (!(UIMgr_txFifoHead == UIMgr_txFifoTail))
#define IS_DATA_IN_RX_FIFO() (!(UIMgr_rxFifoHead == UIMgr_rxFifoTail))

/*  Local Function Declaration */
static unsigned char UIMgr_readRxFifo(void);
void UIMgr_writeTxFifo(unsigned char data);
void onRxFrame(const uint8_t *buffer, uint16_t bytes_received);


static int8_t Version[] = "v1.01 20.09.2013\0";
uint8_t *txmsgptr;

uint8_t rx_head_err=0, rx_crc_err=0, rx_good_err=0, tx_good_err=0, rx_len_err=0;
static uint8_t tmpoutlen;

void UIMgr_init(void)
{
	/*memset(asciiTokenBuffer,0x00,MAX_TOKEN_LENGTH+1);
	memset(tokenBuffer,0x00,MAX_TOKEN_COUNT);*/

	memset(UIMgr_txFifo,0x00,UI_MGR_TX_FIFO_SIZE);
	memset(UIMgr_rxFifo,0x00,UI_MGR_RX_FIFO_SIZE);
	txmsgptr = txmsg;
	hdlc_init(&UIMgr_writeTxFifo, &onRxFrame);
}
/***********************************************************
	Function Name: UIMgr_dispatchEvent
	Function Description: This function is responsible for
	processing events that pertain to the UIMgr.
	Inputs:  event - the generated event
	Outputs: none
***********************************************************/	
void UIMgr_dispatchEvent(uint8_t event)
{
	switch(event)	{
		/*case EV_ACQUIRE_LINE_COMPLETE:
			UIMgr_transmitPendingData();
		break;
		*/
		case EV_SERIAL_DATA_RECEIVED:
			UIMgr_processReceivedData();
		break;
		
		case EV_SERIAL_DATA_PENDING_TX:
			UIMgr_transmitPendingData();
		break;
	}
}

void onRxFrame(const uint8_t *buffer, uint16_t bytes_received)
{
	int8_t *pData;
	static uint8_t setIdx;
	static uint8_t TotalLength;
	static uint8_t sel_drive;
	
	BIT_XOR(PORTD, 7);
	
	if(buffer[1] != bytes_received){
		rx_len_err++;
	}
	else {
	switch (buffer[0])	{
		case VERSION_CMD:	// return version ID
			rx_good_err++;
			pData = Version;
			txmsgptr = txmsg + 2;
			txmsg[0] = VERSION_ACK_CMD;
			txmsg[1] = VERSION_ACK_CMD_LEN;
			while(*pData != 0){
				*txmsgptr++ = *pData++;
			}
			tx_good_err++;
			hdlc_tx_frame(txmsg, VERSION_ACK_CMD_LEN);
			PUBLISH_EVENT(EV_SERIAL_DATA_PENDING_TX); 
			break;
		case PING_CMD:		// ping CMD ACK
			rx_good_err++;
			txmsg[0] = PING_ACK_CMD;
			txmsg[1] = PING_ACK_CMD_LEN;
			txmsg[2] = buffer[2]+1;
			tx_good_err++;
			hdlc_tx_frame(txmsg, VERSION_ACK_CMD_LEN);
			PUBLISH_EVENT(EV_SERIAL_DATA_PENDING_TX); 
			break;
		case STAT_CMD:		// return statistics
			rx_good_err++;
			txmsg[0] = STAT_ACK_CMD;
			txmsg[1] = STAT_ACK_CMD_LEN;
			txmsg[2] = buffer[2]+1;
			txmsg[3] = rx_good_err;
			txmsg[4] = rx_head_err;
			txmsg[5] = rx_crc_err;
			txmsg[6] = tx_good_err++;
			hdlc_tx_frame(txmsg, STAT_ACK_CMD_LEN);
			PUBLISH_EVENT(EV_SERIAL_DATA_PENDING_TX); 
			break;
		
		// dvg params
		case WRITE_DVGPRM_EEPROM_CMD:	// write current params to flash
			rx_good_err++;
			setIdx = buffer[3];
			if(setIdx < (DRV_CNT+1)) SaveConfig(setIdx);
			break;
		case LOAD_DVGPRM_EEPROM_CMD:	// load params to current from flash
			rx_good_err++;
			setIdx = buffer[3];
			if(setIdx < (DRV_CNT+1)) LoadConfig(setIdx);
			break;
		case SET_DVGPRM_EEPROM_CMD:		// write received params to current
			rx_good_err++;
			setIdx = buffer[3];
			if(setIdx < (DRV_CNT+1)){
				if(setIdx == 0){ // set config to all drivers
					for(uint8_t i=0;i<DRV_CNT;i++){
						memcpy((drivecfg_t *)&drvCfg[i], (uint8_t *)&buffer[4], DRV_CFG_SIZE);
					}
				} else {
					memcpy((drivecfg_t *)&drvCfg[(setIdx-1)], (uint8_t *)&buffer[4], DRV_CFG_SIZE);
				}
			}
			break;
		case GET_DVGPRM_EEPROM_CMD:		// send current params
			rx_good_err++;
			setIdx = buffer[3];
			if(setIdx < (DRV_CNT+1)){
				TotalLength = DRV_CFG_SIZE + 4;   // body+Header+crc+start/stop
				txmsg[0] = GET_DVGPRM_ACK_CMD;
				txmsg[1] = TotalLength;
				txmsg[2] = buffer[2]+1;
				txmsg[3] = setIdx;
				memcpy((uint8_t*)&txmsg[4], &drvCfg[(setIdx-1)], DRV_CFG_SIZE);
				tx_good_err++;
				hdlc_tx_frame(txmsg, TotalLength);
				PUBLISH_EVENT(EV_SERIAL_DATA_PENDING_TX);
			}
			break;
		case GET_ENC_CMD:		// encoders value request
			rx_good_err++;
			txmsg[0] = GET_ENC_ACK_CMD;
			tmpoutlen = (buffer[3] == 0x0)?11:8;
			// [head, len, cnt, idx, enc0, enc1, enc2, enc3]
			// [head, len, cnt, enc00, enc01, enc02, enc03, enc10, enc11, enc12, enc13]
			txmsg[1] = tmpoutlen;
			txmsg[2] = buffer[2]+1;
			if(buffer[3]==0x1){
				txmsg[3] = 0x1;
				txmsg[4] = (uint8_t)((enc0>>24) & 0xff);
				txmsg[5] = (uint8_t)((enc0>>16) & 0xff);
				txmsg[6] = (uint8_t)((enc0>>8) & 0xff);
				txmsg[7] = (uint8_t)(enc0 & 0xff);
			} else if(buffer[3]==0x2){
				txmsg[3] = 0x2;
				txmsg[4] = (uint8_t)((enc1>>24) & 0xff);
				txmsg[5] = (uint8_t)((enc1>>16) & 0xff);
				txmsg[6] = (uint8_t)((enc1>>8) & 0xff);
				txmsg[7] = (uint8_t)(enc1 & 0xff);
			} else if(buffer[3]==0x0){
				txmsg[3] = (uint8_t)((enc0>>24) & 0xff);
				txmsg[4] = (uint8_t)((enc0>>16) & 0xff);
				txmsg[5] = (uint8_t)((enc0>>8) & 0xff);
				txmsg[6] = (uint8_t)(enc0 & 0xff);
				
				txmsg[7] = (uint8_t)((enc1>>24) & 0xff);
				txmsg[8] = (uint8_t)((enc1>>16) & 0xff);
				txmsg[9] = (uint8_t)((enc1>>8) & 0xff);
				txmsg[10] = (uint8_t)(enc1 & 0xff);
			}
			tx_good_err++;
			hdlc_tx_frame(txmsg, tmpoutlen);
			PUBLISH_EVENT(EV_SERIAL_DATA_PENDING_TX);
			break;
		case GET_SPEED_CMD:		// encoders value request
			rx_good_err++;
			txmsg[0] = GET_SPEED_ACK_CMD;
			tmpoutlen = (buffer[3] == 0x0)?11:8;
			// [head, len, cnt, idx, enc0, enc1, enc2, enc3]
			// [head, len, cnt, enc00, enc01, enc02, enc03, enc10, enc11, enc12, enc13]
			txmsg[1] = tmpoutlen;
			txmsg[2] = buffer[2]+1;
			if(buffer[3]==0x1){
				txmsg[3] = 0x1;
				txmsg[4] = (uint8_t)((speed0>>24) & 0xff);
				txmsg[5] = (uint8_t)((speed0>>16) & 0xff);
				txmsg[6] = (uint8_t)((speed0>>8) & 0xff);
				txmsg[7] = (uint8_t)(speed0 & 0xff);
			} else if(buffer[3]==0x2){
				txmsg[3] = 0x2;
				txmsg[4] = (uint8_t)((speed1>>24) & 0xff);
				txmsg[5] = (uint8_t)((speed1>>16) & 0xff);
				txmsg[6] = (uint8_t)((speed1>>8) & 0xff);
				txmsg[7] = (uint8_t)(speed1 & 0xff);
			} else if(buffer[3]==0x0){
				txmsg[3] = (uint8_t)((speed0>>24) & 0xff);
				txmsg[4] = (uint8_t)((speed0>>16) & 0xff);
				txmsg[5] = (uint8_t)((speed0>>8) & 0xff);
				txmsg[6] = (uint8_t)(speed0 & 0xff);
				
				txmsg[7] = (uint8_t)((speed1>>24) & 0xff);
				txmsg[8] = (uint8_t)((speed1>>16) & 0xff);
				txmsg[9] = (uint8_t)((speed1>>8) & 0xff);
				txmsg[10] = (uint8_t)(speed1 & 0xff);
			}
			tx_good_err++;
			hdlc_tx_frame(txmsg, tmpoutlen);
			PUBLISH_EVENT(EV_SERIAL_DATA_PENDING_TX);
			break;
		case SETENCMODE_CMD:				// set encder mode
			encodersMode = buffer[3];
			switchHCTLMode(encodersMode);
			break;
		case GET_CURR_SNS_CMD:
			rx_good_err++;
			txmsg[0] = GET_CURR_SNS_ACK_CMD;
			tmpoutlen = GET_CURR_SNS_ACK_CMD_LEN;
			// [head, len, cnt, idx, enc0]
			txmsg[1] = tmpoutlen;
			txmsg[2] = buffer[2]+1;
			if(buffer[3]==0x1){
				txmsg[3] = 0x1;
				txmsg[4] = (uint8_t)((tcurSns0>>2) & 0xff);
			} else if(buffer[3]==0x2){
				txmsg[3] = 0x2;
				txmsg[4] = (uint8_t)((tcurSns1>>2) & 0xff);
			}
			tx_good_err++;
			hdlc_tx_frame(txmsg, GET_CURR_SNS_ACK_CMD_LEN);
			PUBLISH_EVENT(EV_SERIAL_DATA_PENDING_TX);
			break;
		case GET_ALL_CURR_SNS_CMD:
			rx_good_err++;
			txmsg[0] = GET_ALL_CURR_SNS_ACK_CMD;
			tmpoutlen = GET_CURR_SNS_ACK_CMD_LEN;
			// [head, len, cnt, enc00, enc10]
			txmsg[1] = tmpoutlen;
			txmsg[2] = buffer[2]+1;
			txmsg[3] = (uint8_t)((tcurSns0>>2) & 0xff);
			txmsg[4] = (uint8_t)((tcurSns1>>2) & 0xff);
			tx_good_err++;
			hdlc_tx_frame(txmsg, GET_CURR_SNS_ACK_CMD_LEN);
			PUBLISH_EVENT(EV_SERIAL_DATA_PENDING_TX);
			break;
		case CALIBDVG_CMD:					// drivers calibration
			/*
			sel_drive = buffer[3];
			lastecodersMode = encodersMode;
			switchHCTLMode(4);
			DriveRst(drv_pins[sel_drive]);
			ResetHCTL(sel_drive);
			*/
			break;			
		case RSTENC_CMD:				// reset encoder
			ResetHCTL(buffer[3]);
			break;
		case SETDVG_CMD:				// set drive speed value
			sel_drive = buffer[3] - 1;
			if(sel_drive == 0){
				UpdatePWM(drv_pins[0], buffer[4], drvCfg[0]);
				UpdatePWM(drv_pins[0], buffer[4], drvCfg[0]);
			}
			else
				UpdatePWM(drv_pins[sel_drive], buffer[4], drvCfg[sel_drive]);
			break;	
		case RSTDVG_CMD:				// reset drivers
			sel_drive = buffer[3] - 1;
			if(sel_drive == 0){
				DriveRst(drv_pins[0]);
				DriveRst(drv_pins[1]);
			}
			else
				DriveRst(drv_pins[sel_drive]);
			break;
		case WINDDVG_CMD:				// set drive wind state
			sel_drive = buffer[3] - 1;
			if(sel_drive == 0){
				drv_pins[0].wind = buffer[4];
				drv_pins[1].wind = buffer[4];
				DriveWind(drv_pins[0]);
				DriveWind(drv_pins[1]);
			}
			else {
				drv_pins[sel_drive].wind = buffer[4];
				DriveWind(drv_pins[sel_drive]);
			}
			break;
		case GET_SYS_CTC:{
			rx_good_err++;
			txmsg[0] = GET_SYS_CTC_ACK_CMD;
			txmsg[1] = GET_SYS_CTC_ACK_CMD_LEN;
			txmsg[2] = buffer[2]+1;
			txmsg[3] = pcDiv & 0x7;
			txmsg[4] = pcOCR;
			tx_good_err++;
			hdlc_tx_frame(txmsg, GET_SYS_CTC_ACK_CMD_LEN);
			PUBLISH_EVENT(EV_SERIAL_DATA_PENDING_TX);
			break;
			}
		case SET_SYS_CTC:{
			// [head, len, cnt, div, ocr]
			StopPerifClk();
			SetPerifClk(buffer[3], buffer[4]);
			break;
		}
		case START_SYS_CTC:{
			StartPerifClk();
			break;
		}
		case STOP_SYS_CTC:{
			StopPerifClk();
			break;
		}
		case ST_ENCEV_CMD:{
			evalEnc = buffer[3];
			evalDvgVal = buffer[4];
			evalPointsCnt = (buffer[5]<<8)|buffer[6];
			tEvalEnc = 0; lEvalEnc = 0;
			
			// allocate memory
			evalPointsInd = 0;
			evalMode = 0;
			evalRdy = 0;
			evalMemory = (uint8_t*)malloc(evalPointsCnt);
			if(evalMemory == 0x0){
				// fail allocate memory				
				evalMode = 0;
				evalEnc = 0;
				evalPointsCnt = 0;
				
				// send message about fail memory allocation
				txmsg[0] = FAIL_ALLOC_MEM_CMD;
				txmsg[1] = FAIL_ALLOC_MEM_CMD_LEN;
				txmsg[2] = buffer[2]+1;
				txmsg[3] = pcDiv & 0x7;
				txmsg[4] = pcOCR;
				tx_good_err++;
				hdlc_tx_frame(txmsg, FAIL_ALLOC_MEM_CMD_LEN);
				PUBLISH_EVENT(EV_SERIAL_DATA_PENDING_TX);
			} else {
				evalMode = 1;				
				ResetHCTL(3);
				UpdatePWM(drv_pins[evalEnc], evalDvgVal, drvCfg[evalEnc]);
			}
			break;
		}

		default:
			rx_head_err++;
			break;
	}
	}//enc0, enc1
}
/*
GET_DRV_SENC_CMD			0x7		// [head, len, cnt, endidx]
#define GET_DRV_AENC_CMD			0x8		// [head, len, cnt]
*/
/***********************************************************
	Function Name: UIMgr_transmitPendingData
	Function Description: This function is responsible for
	transmitting a single byte of data if data is waiting
	to be sent.  Otherwise, if nothing is waiting, the
	function just returns.
	Inputs:  none 
	Outputs: none
***********************************************************/
void UIMgr_transmitPendingData(void)
{
	if (IS_DATA_IN_TX_FIFO() == 1)	{
		/* data is waiting...send a single byte */
		UartInt_txByte(UIMgr_readTxFifo());
	}
}

/***********************************************************
	Function Name: UIMgr_processReceivedData
	Function Description: This function is responsible for
	parsing any serial data waiting in the rx fifo
	Inputs:  none 
	Outputs: none
***********************************************************/
void UIMgr_processReceivedData(void)
{
	while(IS_DATA_IN_RX_FIFO() == 1)	{
		hdlc_on_rx_byte(UIMgr_readRxFifo());
	}
}						
/***********************************************************
	Function Name: UIMgr_readRxFifo
	Function Description: This function is responsible for
	reading a single byte of data from the rx fifo, and
	updating the appropriate pointers.
	Inputs:  none 
	Outputs: unsigned char-the data read
***********************************************************/	
static unsigned char UIMgr_readRxFifo(void)
{
	unsigned char dataByte, tmpTail;
	
	/* just return the current tail from the rx fifo */
	cli();
	dataByte = UIMgr_rxFifo[UIMgr_rxFifoTail];	
	tmpTail = (UIMgr_rxFifoTail+1) & (UI_MGR_RX_FIFO_MASK);
	UIMgr_rxFifoTail = tmpTail;
	sei();
	
	return(dataByte);
}
/***********************************************************
	Function Name: UIMgr_writeTxFifo
	Function Description: This function is responsible for
	writing a single byte to the TxFifo and
	updating the appropriate pointers.
	Inputs:  data - the byte to write to the Fifo 
	Outputs: none
***********************************************************/	
void UIMgr_writeTxFifo(unsigned char data)
{
	unsigned char tmpHead;
	cli();
	UIMgr_txFifo[UIMgr_txFifoHead] = data;

    /* now move the head up */
    tmpHead = (UIMgr_txFifoHead + 1) & (UI_MGR_TX_FIFO_MASK);
    UIMgr_txFifoHead = tmpHead;
	sei();
}

/***********************************************************
	Function Name: UIMgr_readTxFifo
	Function Description: This function is responsible for
	reading a single byte of data from the tx fifo, and
	updating the appropriate pointers.
	Inputs:  none 
	Outputs: unsigned char-the data read
***********************************************************/	
unsigned char UIMgr_readTxFifo(void)
{
	unsigned char dataByte, tmpTail;
	
	/* just return the current tail from the tx fifo */
	cli();
	dataByte = UIMgr_txFifo[UIMgr_txFifoTail];	
	tmpTail = (UIMgr_txFifoTail+1) & (UI_MGR_TX_FIFO_MASK);
	UIMgr_txFifoTail = tmpTail;
	sei();
	
	return(dataByte);
}
/***********************************************************
	Function Name: UIMgr_flushTxBuffer
	Function Description: This function is responsible for
	sending all data currently in the serial tx buffer
	to the user.
	Inputs:  none
	Outputs: none
***********************************************************/	
void UIMgr_flushTxBuffer(void)
{
	while(IS_DATA_IN_TX_FIFO())
		UartInt_txByte(UIMgr_readTxFifo() );
}
