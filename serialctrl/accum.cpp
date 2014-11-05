//---------------------------------------------------------------------------

#include <vcl.h>
#include <math.h>
#pragma hdrstop

#include "accum.h"
#include "main.h"


extern TMainForm *MainForm;
extern CDevice *dev;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAccumForm *AccumForm;
extern int SelDriveIdx;
//---------------------------------------------------------------------------
inline double round( double val )
{
    if( val < 0 ) return ceil(val - 0.5);
    return floor(val + 0.5);
}
//---------------------------------------------------------------------------
__fastcall TAccumForm::TAccumForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TAccumForm::FormCreate(TObject *Sender)
{
    /*for(int i=0;i<DRV_CNT;i++)
        cb1->Items->Add("Motor # " + IntToStr(i+1));
      */
}
//---------------------------------------------------------------------------
void __fastcall TAccumForm::FormShow(TObject *Sender)
{
    Label1->Caption = "Selecected motor: Motor # " + IntToStr(SelDriveIdx);
}
//---------------------------------------------------------------------------
void __fastcall TAccumForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    MainForm->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TAccumForm::SaveBtnClick(TObject *Sender)
{
    if(!sd->Execute())
        return;
    if(sd->FileName == "")
        return;

    SelFilePath = sd->FileName;
    if(sd->FilterIndex == 1)
        rb1->Checked = true;
    else if(sd->FilterIndex == 2)
        rb2->Checked = true;
    Edit3->Text = ExtractFileName(SelFilePath);
    SelFilePath = ExtractFilePath(SelFilePath);
}
//---------------------------------------------------------------------------
void __fastcall TAccumForm::StartBtnClick(TObject *Sender)
{
    const long double F_CPU = 16.0e6f;  // cpu frequecy
//    const int ENC_PROC_CNT = 137;       // get new encoder values cpu cycles
    const int ENC_PROC_CNT = 142;       // get new encoder values cpu cycles
    const long double ENC_PROC_FREQ = F_CPU / (long double)(2*ENC_PROC_CNT+100);
    const long double ENC_PROC_PERIOD = 1.0f/ENC_PROC_FREQ;
    const int MAX_MEMORY = 6000;    // total dynamic memory bytes
    const int CTC_DIV[] = {1, 8, 64, 256, 1024};
    const int CTC_DIV_SIZE = sizeof(CTC_DIV)/sizeof(CTC_DIV[0]);
    long double OcrList[CTC_DIV_SIZE];
    int ocrListCnt = 0;
    int IntegerIndex = -1;
    char ErrBuff[255];
    long double TimeStep = Edit1->Text.ToDouble()/1000.0f;
    long double TimeInterval = Edit2->Text.ToDouble()/1000.0f;

    int selDiv, selOCR;

    int tSpeed = Edit4->Text.ToInt();
    if(tSpeed > 127) tSpeed = 127;
    else if(tSpeed < -127) tSpeed = -127;
    Edit4->Text = IntToStr(tSpeed);
    
    unsigned char rSpeed = tSpeed;
    if(rSpeed & 0x80) rSpeed = 128 - rSpeed;



    // check ferquency
    if(TimeStep < ENC_PROC_PERIOD){
        sprintf(ErrBuff, "Error!\n Interval maus be bigger than %.3Lf ms\0", ENC_PROC_PERIOD*1000.0f);
        ShowMessage(ErrBuff);
        return;
    }

    // check memory size
    int PointsCnt = TimeInterval / TimeStep;
    if(MAX_MEMORY < PointsCnt){
        memset(ErrBuff, 0, 255);
        sprintf(ErrBuff, "Error!\n There are to many points-%.0Lf.\n At this MCU can save only %d points\0", PointsCnt, MAX_MEMORY);
        ShowMessage(ErrBuff);
        return;
    }

    /* calculate OCR values
        Criteries:
        1. Integer
        2. If not Integer -> mi error
    */
    long double Ftcnt = 1.0f/TimeStep;
    long double tOcr, actInt, minError = 1e6, calcInt;
    int minErrIndex, minErrOcr, flOcrValue, minflOcrValue;
    memset(OcrList, 0, sizeof(long double)*ocrListCnt);
    for(int i = CTC_DIV_SIZE-1; i>=0; i--){
        tOcr = F_CPU / (2.0f*Ftcnt*(long double)CTC_DIV[i]) - 1.0f;
        if( (tOcr <= 0.0f) || tOcr > 256){
            OcrList[i] = -1e6;
            continue;
        }
        OcrList[i] = tOcr;

        // test to integer value
        if((fabs(tOcr - (int)tOcr) - 1) >= 0.0f)
            IntegerIndex = i;
        else {
            // Calculate min error if div will be not integer
            flOcrValue = round( tOcr * 1000.0 ) / 1000.0;
            calcInt = 1.0f/(F_CPU/(2.0*(long double)CTC_DIV[i]*(1.0 + flOcrValue)));
            if(fabs(TimeStep - calcInt) < minError){
                minError = fabs(TimeStep - calcInt);
                minErrIndex = i;
                minflOcrValue = flOcrValue;
                actInt = calcInt;
            }
        }
    }

    if(IntegerIndex < 0){
        memset(ErrBuff, 0, 255);
        sprintf(ErrBuff, "Ahtung!\n There are no integer dividers\nActual interval will be %.5Lf ms\nDiv = %d, OCR = %d\0", actInt*1000.0f, CTC_DIV[minErrIndex], minflOcrValue);
        ShowMessage(ErrBuff);

        selDiv = CTC_DIV[minErrIndex];
        selOCR = minflOcrValue;

    } else {
        selDiv = CTC_DIV[IntegerIndex];
        selOCR = flOcrValue/1000;
        /*
        memset(ErrBuff, 0, 255);
        flOcrValue = round( OcrList[IntegerIndex] * 1000.0 );
        sprintf(ErrBuff, "Div = %d, OCR = %d\0", CTC_DIV[IntegerIndex], flOcrValue/1000);
        ShowMessage(ErrBuff);
        */
    }

    // device commands
    dev->CmdGetSysCTC();
    Sleep(2);
    dev->CmdStopCTC();
    Sleep(1);
    byte oldDiv = dev->GetSysDiv();
    byte oldOCR = dev->GetSysOCR();

    dev->CmdSetSysCTC(selDiv, selOCR);
    dev->CmdStartCTC();
    
    // evaluate encoder mode
    dev->CmdEvalEncoder(SelDriveIdx-1, rSpeed, PointsCnt);

    while(!dev->isEvalEncReady()){
        Sleep(1);
    }
    
    // return previous settings
    dev->CmdStopCTC();
    dev->CmdSetSysCTC(oldDiv, oldOCR);
    dev->CmdStartCTC();
    SelFilePath+="//"+Edit3->Text;
    dev->SaveEvalData(SelFilePath.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TAccumForm::StopBtnClick(TObject *Sender)
{
   //
}
//---------------------------------------------------------------------------
