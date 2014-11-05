//---------------------------------------------------------------------------

#ifndef accumH
#define accumH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TAccumForm : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TButton *SaveBtn;
    TLabel *Label4;
    TEdit *Edit1;
    TButton *StartBtn;
    TButton *StopBtn;
    TEdit *Edit2;
    TSaveDialog *sd;
    TCheckBox *cb1;
    TRadioButton *rb1;
    TRadioButton *rb2;
    TLabel *Label5;
    TEdit *Edit3;
    TEdit *Edit4;
    TLabel *Label6;
    TEdit *Edit5;
    TLabel *Label7;
    TEdit *Edit6;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall SaveBtnClick(TObject *Sender);
    void __fastcall StartBtnClick(TObject *Sender);
    void __fastcall StopBtnClick(TObject *Sender);
private:	// User declarations
    AnsiString SelFilePath;
public:		// User declarations
    __fastcall TAccumForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAccumForm *AccumForm;
//---------------------------------------------------------------------------
#endif
