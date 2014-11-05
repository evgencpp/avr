//---------------------------------------------------------------------------
#ifndef hdeviceH
#define hdeviceH

#include <stdio.h>
#include <vector>
#include <utility>
//#include <string>
//#include <iostream>

using namespace std;

#include "SerialPort.h"
#include "crc16_ccitt.h"
#include "header.h"

// CTC0 define
#define CS02    2
#define CS01    1
#define CS00    0

typedef struct __tagoutmsg{
    byte *data;
    byte length;
}outmsg;


typedef struct __tagdrivecfg_t{
	uint8_t			drvDir;				// drive direction
	uint8_t			encDir;				// encoder direction
    uint16_t		maxI;				// maximum current
    uint16_t		maxPWM_CW;			// maxPWM_CW
    uint16_t		maxPWM_CCW;			// maxPWM_CCW
	uint16_t		minPWM_CW;			// minPWM_CW
	uint16_t		minPWM_CCW;			// minPWM_CW
	
	float			spKp;
	float			spKi;
	float			spKd;
	
	float			posKp;
	float			posKi;
	float			posKd;
}drivecfg_t;

//typedef unsigned long int uint32_t;
//---------------------------------------------------------------------------
const int Max_COMports_to_iterate = 255;

class CDevice{
public:
    CDevice();
    ~CDevice();

    void CmdPing();
    void CmdGetStat();
    void CmdSetDriveSpeed(byte idx, byte speed);
    void CmdDriveBreak(byte idx, byte state);
    void CmdDriveRst(byte idx);
    void CmdGetEnc(byte idx);
    void CmdGetSpeed(byte idx);
    void CmdGetCurrSns(byte idx);
    void CmdRstEnc();

    void CmdSetParams(byte *params, int length, int idx);
    void CmdGetParams(int idx);
    void CmdWriteParams(int idx);
    void CmdReadParams(int idx);
    void CmdGetAllCurrSns(void);

    void CmdGetSysCTC(void);
    void CmdSetSysCTC(int div, byte OCR);
    void CmdStartCTC(void);
    void CmdStopCTC(void);
    void CmdEvalEncoder(byte selEnc, uint8_t Value, uint16_t pointCnt);

    int32_t GetSpeedVal(int idx);

    uint8_t GetCurrSnsVal(int idx);




    int GetAvailablePortList(std::vector<int> *ports);

    bool InitPort(const  char* portnr = "COM1", UINT baud = 115200, char parity = 'N', UINT databits = 8, UINT stopsbits = 1, DWORD dwCommEvents = EV_RXCHAR, UINT nBufferSize = 4096);

    bool StartDeveceMonitor();
    void StopDeviceMonitor();
    bool DeviceOnLine;

    int GetEnc0(){return enc0;};
    int GetEnc1(){return enc1;};
    void GetErrorLog(byte *ErrorArray);
    char *GetResultMsg(void);

    byte GetSysDiv(){return mSysDiv;};
    byte GetSysOCR(){return mSysOCR;};          // system counter values

    bool isEvalEncReady(){return (bool)(mEvalCnt == encdata.size());};
    bool SaveEvalData(const char *FileName);
protected:
    // Eval encoder system
//    vector<byte>encdata;
    std::vector<std::pair<int, int> > encdata;
    bool mEvalEncMode;
    uint16_t mEvalCnt;

    ///
    char ccmsg[255];

    uint32_t enc0, enc1;
    int32_t speed0, speed1;
    uint8_t snsCurr0, snsCurr1;

    bool SetRxThreadAlived;

    int OnlinePingCnt;
    CRITICAL_SECTION m_csTXSync;
    byte mSysDiv, mSysOCR;          // system counter values 
    byte devTxCnt; 
    vector<outmsg*>TxData;
    CSerialPort *sp;
    //Rx events
/*    HANDLE mRXShutdownEvent;
    HANDLE RXthreadEventArray[2];*/
    HANDLE mDeviceThread;
    bool RxThreadAlived;
    DWORD dwTXThreadId;

    //Tx events
/*    HANDLE mTXEvent;
    HANDLE mTXShutdownEvent;
    HANDLE TXthreadEventArray[2];    */
    HANDLE mTXThreadHand;
/*    bool TxThreadAlived;         */
    bool TxThreadAlived;
    bool SetTxThreadAlived;
    bool isMonitor;

    //hdlc protocol level
    //void GetTxHDLCFrame(outmsg *msg);
    outmsg *GetTxHDLCFrame(byte *buffer, int length);
    void ParceHDLCFrame(byte *buffer, int length);
    void OnDeviceRxCommand(byte *buffer, int length);
    void ClearTxEnque();
    static DWORD __stdcall TXThread(LPVOID pParam);

    static DWORD __stdcall RXThread(LPVOID pParam);

    byte dev_rx_good_err, dev_rx_head_err, dev_rx_crc_err, dev_tx_good_err,
        errHead, errPackLen, errCRC, errEscape;


};
//---------------------------------------------------------------------------
#endif
