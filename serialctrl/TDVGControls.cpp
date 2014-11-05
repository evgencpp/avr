//---------------------------------------------------------------------------
#pragma hdrstop
#include "TDVGControls.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
const int CtrlDvgCnt = 1;
//---------------------------------------------------------------------------
TDVGControls::TDVGControls(TWinControl *parent, CDevice *device, int idx): trBar(NULL)
{
    lastSpeed = 0;
    Speed = 0;
    
    driveIDX = idx;
    devPtr = device;
    gb = new TGroupBox(parent);
    gb->Parent = parent;
    gb->Caption = " Drive # " + IntToStr(idx)+" ";
    gb->Width = 240;
    gb->Left = gb->Width*(idx+1);
    gb->Align = alLeft;

    trBar  = (TTrackBar **)calloc(CtrlDvgCnt, sizeof(TTrackBar*));
    trBarLbl  = (TLabel **)calloc(CtrlDvgCnt, sizeof(TLabel*));

    ctrlEditLbl  = (TLabel **)calloc(CtrlDvgCnt, sizeof(TLabel*));
    ctrlEdit  = (TEdit **)calloc(CtrlDvgCnt, sizeof(TEdit*));

    ctrlSet     = (TButton **)calloc(CtrlDvgCnt, sizeof(TButton));
    ctrlGet     = (TButton **)calloc(CtrlDvgCnt, sizeof(TButton));

    cbWind = new TCheckBox(gb);
    cbWind->Parent = gb;
    cbWind->Caption = "Wind";
    cbWind->Left = 46;
    cbWind->Top = 76;
    cbWind->OnClick = WindClick;

    int editTop = 50;
    for(int i=0;i<CtrlDvgCnt;i++){
        trBar[i] = new TTrackBar(gb);
        trBar[i]->Parent = gb;
        //trBar[i]->Orientation = trVertical;
        trBar[i]->Orientation = trHorizontal;
        trBar[i]->Frequency = 8;
        trBar[i]->Min = -127;
        trBar[i]->Max = 127;
        /*
        trBar[i]->Height = 104;
        trBar[i]->Width = 26;
        */
        trBar[i]->Height = 26;
        trBar[i]->Width = 170;
        //trBar[i]->Top = 27;
        //trBar[i]->Left = 4 + i*26;
        trBar[i]->Top = 20 + i*26;
        trBar[i]->Left = 46;
        trBar[i]->ThumbLength = 7;
        trBar[i]->TickMarks = tmBoth;
         /*
        trBarLbl[i] = new TLabel(gb);
        trBarLbl[i]->Parent = gb;
        trBarLbl[i]->Caption = "I" + IntToStr(i+1);
        trBarLbl[i]->Top = 15;
        trBarLbl[i]->Left = 14+i*26;
           */
        int st = 30;
        ctrlEditLbl[i] = new TLabel(gb);
        ctrlEditLbl[i]->Parent = gb;
        ctrlEditLbl[i]->Caption = "speed";// + IntToStr(i+1);
        ctrlEditLbl[i]->Left = 4;
        ctrlEditLbl[i]->Top = editTop + i*st+5;

        ctrlEdit[i] = new TEdit(gb);
        ctrlEdit[i]->Parent = gb;
        ctrlEdit[i]->Left = 46;
        ctrlEdit[i]->Top = editTop + i*st+2;
        ctrlEdit[i]->Width = 80;
        ctrlEdit[i]->Text = "0";

        ctrlSet[i] = new TButton(gb);
        ctrlSet[i]->Parent = gb;
        ctrlSet[i]->Left = 150;
        ctrlSet[i]->Top = editTop + i*st;
        ctrlSet[i]->Width = 30;
        ctrlSet[i]->Caption = "Set";

        ctrlGet[i] = new TButton(gb);
        ctrlGet[i]->Parent = gb;
        ctrlGet[i]->Left = 186;
        ctrlGet[i]->Top = editTop + i*st;
        ctrlGet[i]->Width = 30;
        ctrlGet[i]->Caption = "Get";
    }
    ctrlGet[0]->OnClick = GetCtrl1Click;
//    ctrlGet[1]->OnClick = GetCtrl2Click;
    ctrlSet[0]->OnClick = SetCtrl1Click;
//    ctrlSet[1]->OnClick = SetCtrl2Click;
    trBar[0]->OnChange = TrackBar0Change;

}

//---------------------------------------------------------------------------
TDVGControls::~TDVGControls()
{
    for(int i=0;i<CtrlDvgCnt;i++){
        delete trBar[i]; trBar[i] = NULL;
       // delete trBarLbl[i]; trBarLbl[i] = NULL;
        delete ctrlEditLbl[i]; ctrlEditLbl[i] = NULL;
        delete ctrlEdit[i]; ctrlEdit[i] = NULL;
        delete ctrlSet[i]; ctrlSet[i] = NULL;
        delete ctrlGet[i]; ctrlGet[i] = NULL;
    }
    free(trBar); trBar = NULL;
    delete cbWind; cbWind = NULL;
    //delete trBar; trBar = NULL;
    free(trBarLbl); trBarLbl = NULL;
    free(ctrlEditLbl); ctrlEditLbl = NULL;
    free(ctrlEdit); ctrlEdit = NULL;
    free(ctrlSet); ctrlSet = NULL;
    free(ctrlGet); ctrlGet = NULL;


    delete gb;

}

//---------------------------------------------------------------------------
void __fastcall TDVGControls::SetCtrl1Click(TObject *Sender)
{
    int t = ctrlEdit[0]->Text.ToInt();
    if(t > 127) t = 127;
    else if(t < -127) t = -127;
    ctrlEdit[0]->Text = IntToStr(t);
    trBar[0]->Position = t;

    unsigned char r = trBar[0]->Position;
    if(r & 0x80) r = 128 - r;

    devPtr->CmdSetDriveSpeed(driveIDX, r);
    lastSpeed = Speed = r;
}

//---------------------------------------------------------------------------
void __fastcall TDVGControls::SetCtrl2Click(TObject *Sender)
{

}

//---------------------------------------------------------------------------
void __fastcall TDVGControls::GetCtrl1Click(TObject *Sender)
{

}

//---------------------------------------------------------------------------
void __fastcall TDVGControls::GetCtrl2Click(TObject *Sender)
{

}

//---------------------------------------------------------------------------
void __fastcall TDVGControls::WindClick(TObject *Sender)
{
    if(((TCheckBox*)Sender)->Checked == true){
        devPtr->CmdDriveBreak(driveIDX, 1);
    } else {
        devPtr->CmdDriveBreak(driveIDX, 0);
    }
    devPtr->CmdSetDriveSpeed(driveIDX, Speed);
}

//---------------------------------------------------------------------------
void __fastcall TDVGControls::TrackBar0Change(TObject *Sender)
{
    ctrlEdit[0]->Text = IntToStr(trBar[0]->Position);
    unsigned char r = trBar[0]->Position;
    if(r & 0x80) r = 128 - r;
    Speed = r;
}

//---------------------------------------------------------------------------
void __fastcall TDVGControls::UpdateDiveceSpeed()
{
    if(lastSpeed != Speed){
        devPtr->CmdSetDriveSpeed(driveIDX, Speed);
        lastSpeed = Speed;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

