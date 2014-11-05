//---------------------------------------------------------------------------
#ifndef TDVGControlsH
#define TDVGControlsH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <ComCtrls.hpp>
#include <stdio.h>
#include "hdevice.h"
//---------------------------------------------------------------------------
class TDVGControls{
private:
    TGroupBox *gb;
    TTrackBar **trBar; TLabel **trBarLbl;
    TEdit **ctrlEdit; TLabel **ctrlEditLbl;
    TButton **ctrlSet, **ctrlGet;
    TCheckBox *cbWind;
    int driveIDX;
    CDevice *devPtr;
    unsigned char Speed, lastSpeed;
public:
    TDVGControls(TWinControl *parent, CDevice *device, int idx);
    ~TDVGControls();

    void __fastcall SetCtrl1Click(TObject *Sender);
    void __fastcall SetCtrl2Click(TObject *Sender);
    void __fastcall GetCtrl1Click(TObject *Sender);
    void __fastcall GetCtrl2Click(TObject *Sender);
    void __fastcall WindClick(TObject *Sender);
    void __fastcall TrackBar0Change(TObject *Sender);

    void __fastcall UpdateDiveceSpeed();
};
//---------------------------------------------------------------------------
#endif
