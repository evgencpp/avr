//---------------------------------------------------------------------------
#pragma hdrstop
#include "hdevice.h"
#include "main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
#define MAX_ONLINECNT   5
CDevice::CDevice()
{
    isMonitor = false;
    devTxCnt = 10;

    RxThreadAlived = true;     //Rx shutdown thread flag
    TxThreadAlived = true;     //Rx shutdown thread flag

    OnlinePingCnt = MAX_ONLINECNT;
    DeviceOnLine = false;
    errPackLen = 0;
    speed0 = 0;
    speed1 = 0;
    enc0 = 0;
    enc1 = 0;
    snsCurr0 = 0;
    snsCurr1 = 0;
    mSysDiv = (1 << CS01)|(1 << CS00);
    mSysOCR = 249;

    mEvalEncMode = false;
    mEvalCnt = 0;

    // initialize critical section  
    InitializeCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
CDevice::~CDevice()
{
    int len;
    ClearTxEnque();
}
//---------------------------------------------------------------------------
void CDevice::OnDeviceRxCommand(byte *buffer, int length)
{
    byte selIdx;
    byte arr[255];
    memcpy(arr, buffer, length);
    uint16_t ttt;
	switch(buffer[0]){
		case PING_ACK_CMD:{  // PING PONG COMMAND
			OnlinePingCnt = MAX_ONLINECNT;
			DeviceOnLine = true;
			sprintf(ccmsg, "Get PING - OK");
			break;
		}
        case VERSION_ACK_CMD: {
            memset(arr, '\0', 255);
            memcpy(arr, buffer, length);
            break;
        }
		case STAT_ACK_CMD:{
			dev_rx_good_err = buffer[3];
			dev_rx_head_err = buffer[4];
			dev_rx_crc_err = buffer[5];
			dev_tx_good_err = buffer[6];
			sprintf(ccmsg, "Get STAT - OK");
			break;
		}
		case GET_DVGPRM_ACK_CMD:
			selIdx = buffer[3]-1;
			dvg_prm[selIdx]->LoadData((byte *)&buffer[4], length, true);
			dvg_prm[selIdx]->MemToForm();
			sprintf(ccmsg, "Get DVG prm - OK");
			break;
		case GET_ENC_ACK_CMD:{
			if(buffer[3]==1){
				enc0 = (buffer[4]<<24)|(buffer[5]<<16)|(buffer[6]<<8)|buffer[7];
			}
			else if(buffer[3]==2){
				enc1 = (buffer[4]<<24)|(buffer[5]<<16)|(buffer[6]<<8)|buffer[7];
			}
			sprintf(ccmsg, "GetEncoder - OK");
			break;
		}
        case GET_ALL_CURR_SNS_ACK_CMD:{
            snsCurr0 = buffer[3];
            snsCurr1 = buffer[4];
            break;
        }
        case GET_CURR_SNS_ACK_CMD:{
            if(buffer[3]==1){
                snsCurr0 = buffer[4];
            } else if(buffer[3]==2){
                snsCurr1 = buffer[4];
            }
            sprintf(ccmsg, "Get Curr SNS - OK");
            break;
        }
        case GET_SPEED_ACK_CMD:{
            if(length == 8){
    			if(buffer[3]==1){
	    			speed0 = (buffer[4]<<24)|(buffer[5]<<16)|(buffer[6]<<8)|buffer[7];
		    	}
			    else if(buffer[3]==2){
				    speed1 = (buffer[4]<<24)|(buffer[5]<<16)|(buffer[6]<<8)|buffer[7];
    			}
            } else if(length == 11){
                speed0 = (buffer[3]<<24)|(buffer[4]<<16)|(buffer[5]<<8)|buffer[6];
                speed1 = (buffer[7]<<24)|(buffer[8]<<16)|(buffer[9]<<8)|buffer[10];
            }
			sprintf(ccmsg, "GetSpeed - OK");
			break;
		}

        case GET_SYS_CTC_ACK_CMD:{
            //  [head, len, cnt, div, ocr]
            mSysDiv = buffer[3];
            mSysOCR = buffer[4];
            break;
        }
        case FAIL_ALLOC_MEM_CMD:{
            ShowMessage("Error memory allocation");
//            	MessageBox(NULL, Temp, "Application Error", MB_ICONSTOP);
            break;
        }
        case EVAL_ENC_DATA_CMD:{
            ttt = (buffer[3]<<8)|buffer[4];
            encdata.push_back(std::make_pair(ttt, buffer[5]));
            break;
        }
		default:
			errHead++;
            break;
	}
}
//---------------------------------------------------------------------------
uint8_t CDevice::GetCurrSnsVal(int idx)
{
    if(idx == 0)
        return snsCurr0;
    else if(idx == 1)
        return snsCurr1;

    return 0x0;
}
//---------------------------------------------------------------------------
int32_t CDevice::GetSpeedVal(int idx)
{
    if(idx == 0)
        return speed0;
    else if(idx == 1)
        return speed1;
        
    return 0x0;
}
//---------------------------------------------------------------------------
char *CDevice::GetResultMsg(void)
{
    return ccmsg;
}

//---------------------------------------------------------------------------
void CDevice::ClearTxEnque()
{
    vector<outmsg*>::iterator it;
    for (it=TxData.begin(); it!=TxData.end(); ++it){
        delete [] (*it)->data;
        (*it)->data = NULL;
        delete (*it); (*it) = NULL;
    }
    TxData.clear();
}
//---------------------------------------------------------------------------
//#define  DEBUG  1
void CDevice::CmdPing()
{
    if(!isMonitor) return;
    byte msg[PING_CMD_LEN];
    msg[0] = PING_CMD;
    msg[1] = PING_CMD_LEN;
    msg[2] = devTxCnt++;
    outmsg *tmp = GetTxHDLCFrame(msg, PING_CMD_LEN);
    OnlinePingCnt--;
    if(OnlinePingCnt<1)
        DeviceOnLine = false;

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
void CDevice::CmdGetStat()
{
    if(!isMonitor) return;
    byte msg[STAT_CMD_LEN];
    msg[0] = STAT_CMD;
    msg[1] = STAT_CMD_LEN;
    msg[2] = devTxCnt++;
    outmsg *tmp = GetTxHDLCFrame(msg, STAT_CMD_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
void CDevice::CmdSetDriveSpeed(byte idx, byte speed)
{
    if(!isMonitor) return;
    byte msg[SETDVG_CMD_LEN];
    msg[0] = SETDVG_CMD;
    msg[1] = SETDVG_CMD_LEN;
    msg[2] = devTxCnt++;
    msg[3] = idx;
    msg[4] = speed;
    outmsg *tmp = GetTxHDLCFrame(msg, SETDVG_CMD_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
void CDevice::CmdDriveBreak(byte idx, byte state)
{
    byte msg[WINDDVG_CMD_LEN];
    msg[0] = WINDDVG_CMD;
    msg[1] = WINDDVG_CMD_LEN;
    msg[2] = devTxCnt++;
    msg[3] = idx;
    msg[4] = state;
    outmsg *tmp = GetTxHDLCFrame(msg, WINDDVG_CMD_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
void CDevice::CmdDriveRst(byte idx)
{
    if(!isMonitor) return;
    byte msg[RSTDVG_CMD_LEN];
    msg[0] = RSTDVG_CMD;
    msg[1] = RSTDVG_CMD_LEN;
    msg[2] = devTxCnt++;
    msg[3] = idx;
    outmsg *tmp = GetTxHDLCFrame(msg, RSTDVG_CMD_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
void CDevice::CmdGetEnc(byte idx)
{
    byte msg[GET_ENC_CMD_LEN];
    msg[0] = GET_ENC_CMD;
    msg[1] = GET_ENC_CMD_LEN;
    msg[2] = devTxCnt++;
    msg[3] = idx;
    outmsg *tmp = GetTxHDLCFrame(msg, GET_ENC_CMD_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
void CDevice::CmdEvalEncoder(byte selEnc, uint8_t Value, uint16_t pointCnt)
{
    encdata.clear();
    mEvalEncMode = true;
    mEvalCnt = pointCnt;

    byte msg[ST_ENCEV_CMD_LEN];
    msg[0] = ST_ENCEV_CMD;
    msg[1] = ST_ENCEV_CMD_LEN;
    msg[2] = devTxCnt++;
    msg[3] = selEnc;
    msg[4] = Value;
    msg[5] = HIBYTE(pointCnt);
    msg[6] = LOBYTE(pointCnt);
    outmsg *tmp = GetTxHDLCFrame(msg, ST_ENCEV_CMD_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);

}
//---------------------------------------------------------------------------
void CDevice::CmdGetCurrSns(byte idx)
{
    byte msg[GET_CURR_SNS_CMD_LEN];
    msg[0] = GET_CURR_SNS_CMD;
    msg[1] = GET_CURR_SNS_CMD_LEN;
    msg[2] = devTxCnt++;
    msg[3] = idx;
    outmsg *tmp = GetTxHDLCFrame(msg, GET_CURR_SNS_CMD_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
void CDevice::CmdGetAllCurrSns(void)
{
    byte msg[GET_ALL_CURR_SNS_CMD_LEN];
    msg[0] = GET_ALL_CURR_SNS_CMD;
    msg[1] = GET_ALL_CURR_SNS_CMD_LEN;
    msg[2] = devTxCnt++;
    outmsg *tmp = GetTxHDLCFrame(msg, GET_ALL_CURR_SNS_CMD_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
void CDevice::CmdGetSpeed(byte idx)
{
    byte msg[GET_ENC_CMD_LEN];
    msg[0] = GET_SPEED_CMD;
    msg[1] = GET_ENC_CMD_LEN;
    msg[2] = devTxCnt++;
    msg[3] = idx;
    outmsg *tmp = GetTxHDLCFrame(msg, GET_ENC_CMD_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
void CDevice::CmdRstEnc()
{
    if(!isMonitor) return;

    byte msg[RSTENC_CMD_LEN];
    msg[0] = RSTENC_CMD;
    msg[1] = RSTENC_CMD_LEN;
    msg[2] = devTxCnt++;
    outmsg *tmp = GetTxHDLCFrame(msg, RSTENC_CMD_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);

}
//---------------------------------------------------------------------------
void CDevice::CmdGetSysCTC(void)
{
    if(!isMonitor) return;

    byte msg[GET_SYS_CTC_LEN];
    msg[0] = GET_SYS_CTC;
    msg[1] = GET_SYS_CTC_LEN;
    msg[2] = devTxCnt++;
    outmsg *tmp = GetTxHDLCFrame(msg, GET_SYS_CTC_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
void CDevice::CmdSetSysCTC(int div, byte OCR)
{
    if(!isMonitor) return;

    byte msg[SET_SYS_CTC_LEN];
    msg[0] = SET_SYS_CTC;
    msg[1] = SET_SYS_CTC_LEN;
    msg[2] = devTxCnt++;
    msg[3] = 0;
    switch(div){
        case 1:
            msg[3] |=                               (1 << CS00);
            break;
        case 8:
            msg[3] |=               (1 << CS01);
            break;
        case 64:
            msg[3] |=               (1 << CS01)|    (1 << CS00);
            break;
        case 256:
            msg[3] |= (1 << CS02);
            break;
        case 1024:
            msg[3] |= (1 << CS02)|                  (1 << CS00);
            break;
    }
    msg[4] = OCR; 
    outmsg *tmp = GetTxHDLCFrame(msg, SET_SYS_CTC_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
void CDevice::CmdStartCTC(void)
{
    if(!isMonitor) return;

    byte msg[START_SYS_CTC_LEN];
    msg[0] = START_SYS_CTC;
    msg[1] = START_SYS_CTC_LEN;
    msg[2] = devTxCnt++;
    outmsg *tmp = GetTxHDLCFrame(msg, START_SYS_CTC_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
void CDevice::CmdStopCTC(void)
{
    if(!isMonitor) return;

    byte msg[STOP_SYS_CTC_LEN];
    msg[0] = STOP_SYS_CTC;
    msg[1] = STOP_SYS_CTC_LEN;
    msg[2] = devTxCnt++;
    outmsg *tmp = GetTxHDLCFrame(msg, STOP_SYS_CTC_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}
//---------------------------------------------------------------------------
void CDevice::CmdSetParams(byte *params, int length, int idx)
{
    if(!isMonitor) return;
    int TotalLength;
    byte msg[512];
    //head, len, cnt, idx,
    TotalLength = length + 4;   // body+Header
    msg[0] = SET_DVGPRM_EEPROM_CMD;
    msg[1] = TotalLength;
    msg[2] = this->devTxCnt++;
    msg[3] = idx;       // drive ID
    memcpy((byte*)&msg[4], params, length);
    outmsg *tmp = GetTxHDLCFrame(msg, TotalLength);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}

//---------------------------------------------------------------------------
void CDevice::CmdGetParams(int idx)
{
    if(!isMonitor) return;
    byte msg[FLASH_PARAMS_CMD_LEN];
    msg[0] = GET_DVGPRM_EEPROM_CMD;
    msg[1] = FLASH_PARAMS_CMD_LEN;
    msg[2] = devTxCnt++;
    msg[3] = idx;
    outmsg *tmp = GetTxHDLCFrame(msg, FLASH_PARAMS_CMD_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}

//---------------------------------------------------------------------------
void CDevice::CmdWriteParams(int idx)
{
    if(!isMonitor) return;
    byte msg[FLASH_PARAMS_CMD_LEN];
    msg[0] = WRITE_DVGPRM_EEPROM_CMD;
    msg[1] = FLASH_PARAMS_CMD_LEN;
    msg[2] = devTxCnt++;
    msg[3] = idx;
    outmsg *tmp = GetTxHDLCFrame(msg, FLASH_PARAMS_CMD_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}

//---------------------------------------------------------------------------
void CDevice::CmdReadParams(int idx)
{
    if(!isMonitor) return;
    byte msg[FLASH_PARAMS_CMD_LEN];
    msg[0] = LOAD_DVGPRM_EEPROM_CMD;
    msg[1] = FLASH_PARAMS_CMD_LEN;
    msg[2] = devTxCnt++;
    msg[3] = idx;
    outmsg *tmp = GetTxHDLCFrame(msg, FLASH_PARAMS_CMD_LEN);

    EnterCriticalSection(&m_csTXSync);
    TxData.push_back(tmp);
    LeaveCriticalSection(&m_csTXSync);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CDevice::StartDeveceMonitor()
{
    OnlinePingCnt = MAX_ONLINECNT;
    DeviceOnLine = false;

    RxThreadAlived = false;
    SetRxThreadAlived = false;
    TxThreadAlived = false;
    SetTxThreadAlived = false;

    errPackLen = 0;
    errHead = 0;
    errCRC = 0;

    ClearTxEnque();
    if(sp)sp->StartMonitoring();

    if ((mDeviceThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&this->RXThread, (LPVOID)this, 0, 0))==NULL)
        return false;

    if ((mTXThreadHand = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&this->TXThread, (LPVOID)this, 0, &dwTXThreadId))==NULL)
        return false;

    isMonitor = true;        
    return true;
}
//---------------------------------------------------------------------------
void CDevice::StopDeviceMonitor()
{

    while(!TxThreadAlived){
        SetTxThreadAlived = true;
        Sleep(1);
    }
        CloseHandle(mTXThreadHand);


    while(!RxThreadAlived){
        SetRxThreadAlived = true;
        Sleep(1);
    }
        CloseHandle(mDeviceThread);

    if(sp){
        sp->StopMonitoring();
        delete sp; sp = NULL;
    }

    OnlinePingCnt = MAX_ONLINECNT;
    DeviceOnLine = false;
    errPackLen = 0;

    isMonitor = false;
    ClearTxEnque();
}

//---------------------------------------------------------------------------
int RxLength;
vector<byte>RxBuffer;
DWORD __stdcall CDevice::RXThread(LPVOID pParam)
{
    CDevice *dev = (CDevice*)pParam;
    dev->RxThreadAlived = false;
    for(;;){
        if(dev->SetRxThreadAlived){
            dev->RxThreadAlived = true;
            ExitThread(0);
            break;
        } else {
        dev->sp->GetRxBuffer(&RxBuffer);
        RxLength = RxBuffer.size();
        if(RxLength>0){
            dev->ParceHDLCFrame((byte*)&RxBuffer[0], RxLength);
        }
        RxBuffer.clear();
        Sleep(1);
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
DWORD __stdcall CDevice::TXThread(LPVOID pParam)
{
    DWORD dwRet;
    CDevice *dev = (CDevice*)pParam;
    int tlen;
            int ln;
            int i;
    dev->TxThreadAlived = false;
    for(;;){
        if(dev->SetTxThreadAlived){
            dev->TxThreadAlived = true;
            ExitThread(0);
            break;
        } else {
            ln = dev->TxData.size();
            i=0;
            while(i<ln){
                dev->sp->WriteToPort(dev->TxData[i]->data, dev->TxData[i]->length);
                //delete[] dev->TxData[i];
                delete dev->TxData[i];
                i++;
            }
            EnterCriticalSection(&dev->m_csTXSync);
            dev->TxData.erase(dev->TxData.begin(), dev->TxData.begin()+ln);
            LeaveCriticalSection(&dev->m_csTXSync);
        }
        Sleep(1);
    }
    return 0;
}
//---------------------------------------------------------------------------
//HDLC low level protocol frame
outmsg *CDevice::GetTxHDLCFrame(byte *buffer, int length)
{
    //[HEAD, MSG0, MSG1, ... , LOW_CRC, HIGH_CRC, TAIL]
    byte *arr = new byte[2*length+50];
    memset(arr, 0, (2*length+50));
    outmsg *msg = new outmsg();

    byte *arrptr = arr;
    int rlen;
    //unsigned short int fcs = CRC16_CCITT_INIT_VAL;
    unsigned short int fcs = HDLC_INITFCS;

   // Start marker
   *arrptr++=HDLC_FLAG_SEQUENCE;


    int bytes_to_send = length;
    // Send escaped data
    while(bytes_to_send)    {
        // Get next data
        *arrptr = *buffer++;

        // Update checksum
        fcs = crc16_ccitt_calc_byte(fcs, *arrptr);
        
        // See if data should be escaped
        if((*arrptr == HDLC_CONTROL_ESCAPE) || (*arrptr == HDLC_FLAG_SEQUENCE)){
            *arrptr++=HDLC_CONTROL_ESCAPE;
            *arrptr ^= HDLC_ESCAPE_BIT;
        }
        
        // add data
        arrptr++;

        // decrement counter
        bytes_to_send--;
    }

    // Invert checksum
    fcs ^= 0xffff;

    // Low byte of inverted FCS
    *arrptr = U16_LO8(fcs);
    if((*arrptr == HDLC_CONTROL_ESCAPE) || (*arrptr == HDLC_FLAG_SEQUENCE)){
        *arrptr++=HDLC_CONTROL_ESCAPE;
        *arrptr ^= HDLC_ESCAPE_BIT;
    }
    arrptr++;

    // High byte of inverted FCS
    *arrptr = U16_HI8(fcs);
    if((*arrptr == HDLC_CONTROL_ESCAPE) || (*arrptr == HDLC_FLAG_SEQUENCE)){
        *arrptr++=HDLC_CONTROL_ESCAPE;
        *arrptr ^= HDLC_ESCAPE_BIT;
    }
    arrptr++;
    
    // End marker
    *arrptr++=HDLC_FLAG_SEQUENCE;

    rlen = arrptr - arr;
    
    // create new message
    msg->data = new byte[rlen];
    memcpy(msg->data, arr, rlen);
    msg->length = rlen;
    
    return msg;
}
//---------------------------------------------------------------------------
#define RX_BUF_LEN  4096
byte hdlc_rx_frame[RX_BUF_LEN];
void CDevice::ParceHDLCFrame(byte *buffer, int length)
{
    int clen = length;
    byte *bptr = buffer;

    static bool hdlc_rx_char_esc = FALSE;
    static unsigned short int hdlc_rx_frame_fcs = CRC16_CCITT_INIT_VAL;
    static unsigned short int hdlc_rx_frame_index = 0;
    
    while(clen){
          // Start/End sequence
        if(*bptr == HDLC_FLAG_SEQUENCE){
            // If Escape sequence + End sequence is received then this packet must be silently discarded
            if(hdlc_rx_char_esc == TRUE){
                hdlc_rx_char_esc = FALSE;
                errEscape++;
            }

            //  Minimum requirement for a valid frame is reception of good FCS
            else if(  (hdlc_rx_frame_index >= sizeof(hdlc_rx_frame_fcs))
                    &&(hdlc_rx_frame_fcs   == CRC16_CCITT_MAGIC_VAL    )  ) {
                // Pass on frame with FCS field removed
                // call proc function
                OnDeviceRxCommand(hdlc_rx_frame,(u8_t)(hdlc_rx_frame_index-2));
            } else {
                errCRC++;
            }
            // Reset for next packet
            hdlc_rx_frame_index = 0;
            hdlc_rx_frame_fcs   = CRC16_CCITT_INIT_VAL;
            clen--; bptr++;
            continue;
        }

        // Escape sequence processing
        if(hdlc_rx_char_esc)    {
            hdlc_rx_char_esc  = FALSE;
            *bptr             ^= HDLC_ESCAPE_BIT;
        }
        else if(*bptr == HDLC_CONTROL_ESCAPE)    {
            hdlc_rx_char_esc = TRUE;
            clen--; bptr++;
            continue;
        }

        // Store received data
        hdlc_rx_frame[hdlc_rx_frame_index] = *bptr;

        // Calculate checksum
        hdlc_rx_frame_fcs =  crc16_ccitt_calc_byte(hdlc_rx_frame_fcs, *bptr);

        // Go to next position in buffer
        hdlc_rx_frame_index++;

        // Check for buffer overflow
        if(hdlc_rx_frame_index == RX_BUF_LEN) {
            // Wrap index
            hdlc_rx_frame_index  = 0;

            // Invalidate FCS so that packet will be rejected
            hdlc_rx_frame_fcs  ^= 0xFFFF;
        }
        bptr++;
        clen--;
    }
}
//---------------------------------------------------------------------------
bool CDevice::InitPort(const  char* portnr,		// portnumber (1..4)
						   UINT  baud,			// baudrate
						   char  parity,		// parity 
						   UINT  databits,		// databits 
						   UINT  stopbits,		// stopbits 
						   DWORD dwCommEvents,	// EV_RXCHAR, EV_CTS etc
						   UINT  writebuffersize)	// size to the writebuffer

{
    sp = new CSerialPort();
    return sp->InitPort(portnr, baud, parity, databits, stopbits, dwCommEvents, writebuffersize);
}
//---------------------------------------------------------------------------
int CDevice::GetAvailablePortList(std::vector<int> *ports)
{
    char szComPort[20];
    HANDLE hCom = NULL;
    int portCnt = 0;
    for (int i = 1; i <= Max_COMports_to_iterate; ++i)  {
        if (i < 10)
            sprintf(szComPort, "COM%d\0", i);
        else
            sprintf(szComPort, "\\\\.\\COM%d\0", i);
 
        hCom = CreateFile(szComPort,  
            GENERIC_READ|GENERIC_WRITE, // desired access should be read&write  
            0,                          // COM port must be opened in non-sharing mode  
            NULL,                       // don't care about the security  
            OPEN_EXISTING,              // IMPORTANT: must use OPEN_EXISTING for a COM port  
            0,                          // usually overlapped but non-overlapped for existance test  
            NULL);                      // always NULL for a general purpose COM port  
 
        if (INVALID_HANDLE_VALUE == hCom){

        }
        else    {
            ports->push_back(i);
            portCnt++;
            CloseHandle(hCom);  
        }  
    }
    return portCnt;
}
//---------------------------------------------------------------------------
void CDevice::GetErrorLog(byte *ErrorArray)
{
    ErrorArray[0] = devTxCnt;
    ErrorArray[1] = errHead;
    ErrorArray[2] = errPackLen;
    ErrorArray[3] = errCRC;
    ErrorArray[4] = errEscape;

    ErrorArray[5] = dev_rx_good_err;
    ErrorArray[6] = dev_rx_head_err;
    ErrorArray[7] = dev_rx_crc_err;
    ErrorArray[8] = dev_tx_good_err;
}
//---------------------------------------------------------------------------
bool CDevice::SaveEvalData(const char *FileName)
{
    std::vector<std::pair<int, int> >:: const_iterator i;
    FILE *out = fopen(FileName, "w+");
    if(!out)
        return false;
    for(i=encdata.begin(); i != encdata.end();++i )   {
        fprintf(out, "%d;%d;\n", i->first, i->second);
    }
    fclose (out);

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------


