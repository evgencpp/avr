//---------------------------------------------------------------------------
#pragma hdrstop
#include "TDVGParamList.h"
#include "main.h"
#include "accum.h"
extern TMainForm *MainForm;
//---------------------------------------------------------------------------
#pragma package(smart_init)
int SelDriveIdx;
//---------------------------------------------------------------------------
TDVGParamList::TDVGParamList(TWinControl *parent, CDevice *device
                , int PrmCount, const char *str[], int idx)
{
    driveIDX = idx;
    prmCnt = PrmCount;
    devPtr = device;
    params = (TDVGParam **) calloc(prmCnt, sizeof(TDVGParam));
    int prmTop = 18;
    prmSpaceCnt=0;

    gb = new TGroupBox(parent);
    gb->Parent = parent;
    gb->Top = 0;
    gb->Align = alLeft;
    gb->Width = 230;
    gb->Left = idx*gb->Width+1;
    gb->Caption = " Drive # "+IntToStr(idx)+ " ";

    // make check box
    EnableCh = new TCheckBox(gb);
    EnableCh->Parent = gb;
    EnableCh->Top = prmTop;
    EnableCh->Left = 10;
    EnableCh->Caption = "Enable drive";
    EnableCh->Checked = true;
    EnableCh->OnClick = EnChClick;
    prmTop += 20;

    EnableAuto = new TCheckBox(gb);
    EnableAuto->Parent = gb;
    EnableAuto->Top = prmTop;
    EnableAuto->Left = 10;
    EnableAuto->Width = 180;
    EnableAuto->Caption = "Enable auto setup";
    EnableAuto->Checked = false;
    EnableAuto->OnClick = AutoSetClick;
    prmTop += 20;

    // make all params
    for(int i=0;i<prmCnt;i++){
        params[i] = (TDVGParam*)new TDVGParam(gb, str[i], prmTop);
        prmTop += params[i]->GetHeight()+4;
        prmSpaceCnt += params[i]->GetPrmLength();
    }
    prmSpace = (byte*)calloc(prmSpaceCnt, 1);

    // init control buttons
    TButton **btn[]={&GetMCUParamsBtn, &SetMCUParamsBtn, &AccumDataBtn,
            &ReadFlashParamsBtn, &WriteFlashParamsBtn,
            &AutoSetupBtn};
    const int CtrlBtnSize = sizeof(btn)/sizeof(btn[0]);
    for(int i=0;i<CtrlBtnSize;i++){
        (*btn[i]) = new TButton(gb);
        (*btn[i])->Parent = gb;
        (*btn[i])->Top = prmTop+4;
        (*btn[i])->Left = (i>2)?135:35;
        if(i==2)
            prmTop -= 3*((*btn[i])->Height+4);
        prmTop += (*btn[i])->Height+4;
    }
    GetMCUParamsBtn->Caption = "Get Params";
    SetMCUParamsBtn->Caption = "Set Params";
    ReadFlashParamsBtn->Caption = "Read Flash";
    WriteFlashParamsBtn->Caption = "Write Flash";
    AccumDataBtn->Caption = "Accum data";
    AutoSetupBtn->Enabled = false;
    AutoSetupBtn->Caption = "Auto setup";

    // Buttons actions
    GetMCUParamsBtn->OnClick = GetMCUPrmClick;
    SetMCUParamsBtn->OnClick = SetMCUPrmClick;
    ReadFlashParamsBtn->OnClick = ReadFlashPrmClick;
    WriteFlashParamsBtn->OnClick = WriteFlashPrmClick;
    AutoSetupBtn->OnClick = AutoSetupPrmClick;
    AccumDataBtn->OnClick = AccumDataPrmClick;

    // set data pointers in global area
    int mpl = 0;
    for(int i=0;i<prmCnt;i++)   {
        params[i]->SetMemPtr(prmSpace + mpl);
        mpl += params[i]->GetPrmLength();
    }
}
//---------------------------------------------------------------------------
void TDVGParamList::MemToForm()
{
    for(int i=0;i<prmCnt;i++)
        params[i]->Mem2Form();
}
//---------------------------------------------------------------------------
void TDVGParamList::FormToMem()
{
    for(int i=0;i<prmCnt;i++)
        params[i]->Form2Mem();
}
//---------------------------------------------------------------------------
void TDVGParamList::GetPrmMemory(byte *memptr)
{
    memcpy(memptr, prmSpace, prmSpaceCnt);
}
//---------------------------------------------------------------------------
TDVGParamList::~TDVGParamList()
{
    gb->Visible = false;
    for(int i=0;i<prmCnt;i++){
        delete params[i];
        params[i] = NULL;
    }
    free(params); params = NULL;
    free(prmSpace); prmSpace = NULL;

    delete GetMCUParamsBtn; GetMCUParamsBtn = NULL;
    delete SetMCUParamsBtn; SetMCUParamsBtn = NULL;
    delete ReadFlashParamsBtn; ReadFlashParamsBtn = NULL;
    delete WriteFlashParamsBtn; WriteFlashParamsBtn = NULL;
    delete AutoSetupBtn; AutoSetupBtn = NULL;
    delete EnableCh; EnableCh = NULL;
    delete EnableAuto; EnableAuto = NULL;
    delete gb; gb = NULL;
}
//---------------------------------------------------------------------------
void TDVGParamList::LoadData(byte *memPtr, long memSize, bool HW)
{
    int mcnt;
    if(HW){
        byte t0;
        WORD val0 = 0;
        DWORD val1 = 0;
        float fval = 0.0;
        for(int i=0;i<prmCnt;i++){
            if(params[i]->isHWPrm()){
                switch(params[i]->GetType()){
                    case dtByte:
                        t0 = 0;
                        *((byte*)(&t0) + 0) = *(memPtr + 0);
                        params[i]->SetByte(t0);
                        memPtr += sizeof(byte);
                        break;
                    case dtWord:
                        val0 = 0;
                        *((byte*)(&val0) + 1) = *(memPtr + 1);
                        *((byte*)(&val0) + 0) = *(memPtr + 0);
                        params[i]->SetWord(val0);
                        memPtr += sizeof(WORD);
                        break;
                    case dtDWord:
                        *((byte*)(&val1) + 3) = *(memPtr + 3);
                        *((byte*)(&val1) + 2) = *(memPtr + 2);
                        *((byte*)(&val1) + 1) = *(memPtr + 1);
                        *((byte*)(&val1) + 0) = *(memPtr + 0);
                        params[i]->SetDWord(val1);
                        memPtr += sizeof(DWORD);
                        break;
                    case dtFloat:
                        fval = 0;
                        *((byte*)(&fval) + 3) = *(memPtr + 3);
                        *((byte*)(&fval) + 2) = *(memPtr + 2);
                        *((byte*)(&fval) + 1) = *(memPtr + 1);
                        *((byte*)(&fval) + 0) = *(memPtr + 0);
                        params[i]->SetDouble(fval);
                        memPtr += sizeof(float);
                        break;
                    default: break;
                }
            }

            /*
            params[i] = (TDVGParam*)new TDVGParam(gb, str[i], prmTop);
            prmTop += params[i]->GetHeight()+4;
            prmSpaceCnt += params[i]->GetPrmLength();
            */
        }
    } else {
        if(prmSpaceCnt == memSize)
            memcpy(prmSpace, memPtr, memSize);
        else
            memset(prmSpace, 0, prmSpaceCnt);
    }
}
//---------------------------------------------------------------------------
void __fastcall TDVGParamList::EnChClick(TObject *Sender)
{
    for(int i=0;i<prmCnt;i++){
        params[i]->SetEnable(EnableCh->Checked, false);
    }
}
//---------------------------------------------------------------------------
void __fastcall TDVGParamList::AutoSetClick(TObject *Sender)
{
    for(int i=0;i<prmCnt;i++){
        params[i]->SetEnable(!EnableAuto->Checked, true);
    }
    AutoSetupBtn->Enabled = EnableAuto->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TDVGParamList::GetMCUPrmClick(TObject *Sender)
{
    devPtr->CmdGetParams(driveIDX);
}
//---------------------------------------------------------------------------
void __fastcall TDVGParamList::SetMCUPrmClick(TObject *Sender)
{
    byte *msg;

    byte v0; WORD v1; DWORD v2; double v4d; float v4;

    int max_datawidth = prmCnt*8;
    int ttl=0;
    msg = (byte *)new byte[max_datawidth];
    memset((byte*)&msg[0], 0, sizeof(byte)*max_datawidth);

    FormToMem();
    drivecfg_t tmpprm;
//    tmpprm.drvDir =
/*
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
const char *dvgprmst[]={
    {"n=drvDir;     t=byte; c=1;    a=0;    hw=1;   e=dropdown['CW', 'CCW']; hit=Drive direction CW/CCW;"},
    {"n=encDir;     t=byte; c=1;    a=0;    hw=1;   e=radio['CW', 'CCW']; hit=Encoder direction CW/CCW;"},
    {"n=maxI;       t=word; c=1;    a=0;    hw=1;   e=edit; hit=MAX current;"},
    {"n=maxPWM_CW;  t=word; c=1;    a=0;    hw=1;   e=edit;"},
    {"n=maxPWM_CCW; t=word; c=1;    a=0;    hw=1;   e=edit;"},
    {"n=encCnt;     t=word; c=1;    a=0;    hw=0;   e=edit;"},
    {"n=gearK;      t=float; c=1;   a=0;    hw=0;   e=edit;"},

    {"n=minPWM_CW;  t=word; c=1;    a=1;    hw=1;   e=edit;"},
    {"n=minPWM_CCW; t=word; c=1;    a=1;    hw=1;   e=edit;"},

    {"n=Speed PID params;       t=byte; c=1;    a=0;    hw=0;   e=delimiter;"},
    {"n=Kp;   t=float; c=1;    a=1;    hw=1;   e=edit;"},
    {"n=Ki;   t=float; c=1;    a=1;    hw=1;   e=edit;"},
    {"n=Kd;   t=float; c=1;    a=1;    hw=1;   e=edit;"},

    {"n=Pos PID params;       t=byte; c=1;    a=0;    hw=0;   e=delimiter;"},
    {"n=Kp;   t=float; c=1;    a=1;    hw=1;   e=edit;"},
    {"n=Ki;   t=float; c=1;    a=1;    hw=1;   e=edit;"},
    {"n=Kd;   t=float; c=1;    a=1;    hw=1;   e=edit;"},
};
*/

    /*
    for(int i=0;i<prmCnt;i++){
        if(params[i]->isHWPrm())
        switch(params[i]->GetType()){
            case dtByte:
                v0 = params[i]->GetByte();
                memcpy((byte*)&msg[ttl], (byte*)&v0, sizeof(byte));
                ttl+=sizeof(byte);
                break;
            case dtWord:
                v1 = params[i]->GetWord();
                memcpy((byte*)&msg[ttl], (byte*)&v1, sizeof(WORD));
                ttl+=sizeof(WORD);
                break;
            case dtDWord:
                v2 = params[i]->GetDWord();
                memcpy((byte*)&msg[ttl], (byte*)&v2, sizeof(DWORD));
                ttl+=sizeof(DWORD);
                break;
            case dtFloat:
                v4d = params[i]->GetDouble();
                v4 = (float)v4d;
                memcpy((byte*)&msg[ttl], (byte*)&v4, sizeof(float));
                ttl+=sizeof(float);
                break;
            default: break;
        }
    }
    */
    devPtr->CmdSetParams(msg, ttl, driveIDX);
    Sleep(10);
    delete msg;msg = NULL;

}
//---------------------------------------------------------------------------
void __fastcall TDVGParamList::ReadFlashPrmClick(TObject *Sender)
{

}
//---------------------------------------------------------------------------
void __fastcall TDVGParamList::WriteFlashPrmClick(TObject *Sender)
{

}
//---------------------------------------------------------------------------
void __fastcall TDVGParamList::AutoSetupPrmClick(TObject *Sender)
{

}
//---------------------------------------------------------------------------
void __fastcall TDVGParamList::AccumDataPrmClick(TObject *Sender)
{
    SelDriveIdx = driveIDX;
    MainForm->Enabled = false;
    AccumForm->Show();
}
//---------------------------------------------------------------------------

