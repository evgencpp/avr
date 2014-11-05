//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"

CDevice *dev;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
TDVGParamList *dvg_prm[DRV_CNT];
TDVGControls *dvg_ctrl[DRV_CNT];
// Name, type, show_catoption, edit_type
// Name, type, use_hw, show_catoption, edit_type
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
const int dvgprmst_size = sizeof(dvgprmst)/sizeof(*dvgprmst);
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
    pc->ActivePageIndex = 0;
}
double ch1Max, ch1Min;
double ch3Max, ch3Min;
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
    //pc->ActivePageIndex = 1;
    RefreshPortList();
    dev = new CDevice();
    ReadIni();

    byte *prmMem;
    int prmMemSize;
    LoadDriveParams("params.cfg", &prmMem, &prmMemSize);
    for(int i=0;i<DRV_CNT;i++)  {
        dvg_prm[i] = (TDVGParamList*)new TDVGParamList(ScrollBox1, dev, dvgprmst_size, dvgprmst, (i+1));
        dvg_prm[i]->LoadData((prmMem+(i*prmMemSize/DRV_CNT)), (prmMemSize/DRV_CNT));
        dvg_prm[i]->MemToForm();

        dvg_ctrl[i] = (TDVGControls *)new TDVGControls(Panel3, dev, (i+1));
    }
    free(prmMem); prmMem = NULL;

    Chart1->BottomAxis->DateTimeFormat = "mm:ss";
    Chart1->LeftAxis->AutomaticMaximum = false;
    Chart1->LeftAxis->AutomaticMinimum = false;
    Chart3->BottomAxis->DateTimeFormat = "mm:ss";
//    Chart3->LeftAxis->AutomaticMaximum = false;
//    Chart3->LeftAxis->AutomaticMinimum = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    SaveIni();
    if(OpenPortBtn1->Caption == "Close"){
        dev->StopDeviceMonitor();
        OpenPortBtn1->Caption = "OpenPort";
        PingPongTimer1->Enabled = false;
        UpdateCtrlTimer->Enabled = false;
        SnsTimer1->Enabled = false;
    }
    delete dev;

    // save params
    byte *prmMem;
    int prmMemSize;
    long onePrmSize = dvg_prm[0]->GetMemLength();
    prmMemSize = onePrmSize * DRV_CNT;
    prmMem = (byte*)calloc(prmMemSize, 1);

    // save params data
    for(int i=0;i<DRV_CNT;i++){
        dvg_prm[i]->FormToMem();
        dvg_prm[i]->GetPrmMemory((byte*)&prmMem[i*onePrmSize]);
    }
    SaveDriveParams("params.cfg", prmMem, prmMemSize);

    for(int i=0;i<DRV_CNT;i++){
        delete dvg_prm[i]; dvg_prm[i] = NULL;
        delete dvg_ctrl[i]; dvg_ctrl[i] = NULL;
    }
    free(prmMem); prmMem = NULL;

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SerialPortListCb1Change(TObject *Sender)
{
    if(SerialPortListCb1->ItemIndex>-1){
        StatusBar1->Panels->Items[0]->Text = "Port: "+SerialPortListCb1->Text;
    } else {
        StatusBar1->Panels->Items[0]->Text = "Port: Not found";
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OpenPortBtn1Click(TObject *Sender)
{
    if(OpenPortBtn1->Caption == "Close"){
        dev->StopDeviceMonitor();
        OpenPortBtn1->Caption = "OpenPort";
        PingPongTimer1->Enabled = false;
        UpdateCtrlTimer->Enabled = false;
        SnsTimer1->Enabled = false;
        SaveIni();
    }
    else {
        if(SerialPortListCb1->ItemIndex>-1){
            if(!dev->InitPort(SerialPortListCb1->Text.c_str(),
                115200,
                'N', 8, 0, EV_RXCHAR,1024
            )){
                ShowMessage("Error open port");
                return;
            } else {
                Sleep(1);
                dev->StartDeveceMonitor();

            	dev->CmdGetSpeed(0);
                dev->CmdGetCurrSns(0);

                OpenPortBtn1->Caption = "Close";
                PingPongTimer1->Enabled = true;
                UpdateCtrlTimer->Enabled = true;
                SnsTimer1->Enabled = true;
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PingPongTimer1Timer(TObject *Sender)
{
    dev->CmdPing();

    if(dev->DeviceOnLine)
        StatusBar1->Panels->Items[1]->Text = "Status: ONline";
    else
        StatusBar1->Panels->Items[1]->Text = "Status: OFFline";
}
//---------------------------------------------------------------------------
TDateTime lastTime;
uint16_t currSnsVals[2];
float currValsf_raw[2];
float currSns_total[2];
float currSns_samples[2];
void __fastcall TMainForm::SnsTimer1Timer(TObject *Sender)
{
    if(dev->DeviceOnLine){

	dev->CmdGetSpeed(0);
    dev->CmdGetAllCurrSns();

	ch1Max = -1e20;
	ch1Min = 1e20;
    ch3Max = -1e20;
	ch3Min = 1e20;
	lastTime = Now() - lastTime;
    TDateTime qq = Now();
    //
	for(int i=0;i<2;i++)	{
		if(Chart1->Series[i]->XValues->Count() == 0){
			Chart1->Series[i]->AddXY(qq, dev->GetSpeedVal(i), "", clRed);
		}
		else {
			Chart1->Series[i]->AddXY(
				Chart1->Series[i]->XValues->Last() +
				lastTime.Val, dev->GetSpeedVal(i), "", clRed);
		}

		if(Chart1->Series[i]->XValues->Count() > 200){
			Chart1->Series[i]->XValues->Delete(0);
			Chart1->Series[i]->YValues->Delete(0);
		}

		if(ch1Max < Chart1->Series[i]->YValues->MaxValue)
			ch1Max  = Chart1->Series[i]->YValues->MaxValue;
		if(ch1Min > Chart1->Series[i]->YValues->MinValue)
			ch1Min  = Chart1->Series[i]->YValues->MinValue;

        currSnsVals[i] = (dev->GetCurrSnsVal(i)<<2) & 0xFFFF;
        currValsf_raw[i] = (currSnsVals[i] * 5.0)/1024.0 - 2.5;

        currSns_total[i] -= currSns_total[i]/2.0;
        currSns_total[i] += currValsf_raw[i];
        float curr_val = (currSns_total[i]/2.0)*10.0;
        // current sensors
        if(Chart3->Series[i]->XValues->Count() == 0){
			Chart3->Series[i]->AddXY(qq, 0.0, "", clRed);
		}
		else {
			Chart3->Series[i]->AddXY(
				Chart3->Series[i]->XValues->Last() +
				lastTime.Val, curr_val, "", clRed);
		}

		if(Chart3->Series[i]->XValues->Count() > 200){
			Chart3->Series[i]->XValues->Delete(0);
			Chart3->Series[i]->YValues->Delete(0);
		}

		if(ch1Max < Chart1->Series[i]->YValues->MaxValue)
			ch1Max  = Chart1->Series[i]->YValues->MaxValue;
		if(ch1Min > Chart1->Series[i]->YValues->MinValue)
			ch1Min  = Chart1->Series[i]->YValues->MinValue;
     /*
		if(ch3Max < Chart3->Series[i]->YValues->MaxValue)
			ch3Max  = Chart3->Series[i]->YValues->MaxValue;
		if(ch3Min > Chart3->Series[i]->YValues->MinValue)
			ch3Min  = Chart3->Series[i]->YValues->MinValue;
            */

	}
    if(Chart1->LeftAxis->Maximum < ch1Min){
        Chart1->LeftAxis->Maximum = ch1Max + 3;
    	Chart1->LeftAxis->Minimum = ch1Min - 3;
    } else {
    	Chart1->LeftAxis->Minimum = ch1Min - 3;
        Chart1->LeftAxis->Maximum = ch1Max + 3;
    }
	/*
    if(Chart3->LeftAxis->Maximum < ch3Min){
        Chart3->LeftAxis->Maximum = ch3Max + .5;
    	Chart3->LeftAxis->Minimum = ch3Min - .5;
    } else {
    	Chart3->LeftAxis->Minimum = ch3Min - .5;
        Chart3->LeftAxis->Maximum = ch3Max + .5;
    }
    */

	lastTime = Now();
    StatusBar1->Panels->Items[2]->Text = (AnsiString)dev->GetResultMsg();
    }
    else {
        Sleep(1);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject *Sender)
{
    if(RunScriptBtn1){
        RunScriptBtn1->Left = Panel7->Width - RunScriptBtn1->Width-2;
        ScriptEdit1->Width = Panel7->Width - RunScriptBtn1->Width-4;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UpdateCtrlTimerTimer(TObject *Sender)
{
    if(pc->ActivePageIndex == 1){
        for(int i=0;i<DRV_CNT;i++)  {
            dvg_ctrl[i]->UpdateDiveceSpeed();
        }
    }
}
//---------------------------------------------------------------------------
 
