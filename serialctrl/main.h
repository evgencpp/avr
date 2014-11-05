//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

#include <Chart.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <CheckLst.hpp>


#include "hdevice.h"
#include "TDVGParamList.h"
#include "TDVGControls.h"
#define DRV_CNT 2

extern TDVGParamList *dvg_prm[DRV_CNT];


//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
    TPageControl *pc;
    TTabSheet *SettingsTab1;
    TTimer *PingPongTimer1;
    TStatusBar *StatusBar1;
    TScrollBox *ScrollBox1;
    TTimer *SnsTimer1;
    TPanel *Panel2;
    TButton *OpenPortBtn1;
    TComboBox *SerialPortListCb1;
    TTabSheet *TabSheet1;
    TPanel *Panel3;
    TPanel *Panel1;
    TPanel *Panel4;
    TSplitter *Splitter1;
    TChart *Chart1;
    TChart *Chart3;
    TFastLineSeries *Series1;
    TFastLineSeries *Series2;
    TTabSheet *TabSheet2;
    TPanel *Panel5;
    TEdit *ScriptEdit1;
    TPanel *Panel6;
    TPanel *Panel7;
    TMemo *Memo1;
    TButton *RunScriptBtn1;
    TCheckListBox *CheckListBox1;
    TTimer *UpdateCtrlTimer;
    TFastLineSeries *Series5;
    TFastLineSeries *Series3;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall SerialPortListCb1Change(TObject *Sender);
    void __fastcall OpenPortBtn1Click(TObject *Sender);
    void __fastcall PingPongTimer1Timer(TObject *Sender);
    void __fastcall SnsTimer1Timer(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall UpdateCtrlTimerTimer(TObject *Sender);
private:	// User declarations

    bool __fastcall ReadIni(void);
    bool __fastcall SaveIni(void);
    //
    void __fastcall RefreshPortList();
    bool __fastcall LoadDriveParams(const char *filename, byte **mem, int *count);
    bool __fastcall SaveDriveParams(const char *filename, byte *mem, int count);

public:		// User declarations
    __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
extern TDVGParamList *dvg_prm[DRV_CNT];
extern TDVGControls *dvg_ctrl[DRV_CNT];
//---------------------------------------------------------------------------
#endif
