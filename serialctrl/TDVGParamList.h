//---------------------------------------------------------------------------
#ifndef TDVGParamListH
#define TDVGParamListH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include "hdevice.h"
#include "TDVGParam.h"
//---------------------------------------------------------------------------
class TDVGParamList
{
protected:
    TDVGParam **params;
    TGroupBox *gb;
    TCheckBox *EnableCh, *EnableAuto;

    int prmCnt;
    CDevice *devPtr;
    int driveIDX;
    // Control buttons
    TButton *GetMCUParamsBtn, *SetMCUParamsBtn,
            *ReadFlashParamsBtn, *WriteFlashParamsBtn,
            *AutoSetupBtn, *AccumDataBtn;

    void __fastcall EnChClick(TObject *Sender);
    void __fastcall AutoSetClick(TObject *Sender);

    void __fastcall GetMCUPrmClick(TObject *Sender);
    void __fastcall SetMCUPrmClick(TObject *Sender);
    void __fastcall ReadFlashPrmClick(TObject *Sender);
    void __fastcall WriteFlashPrmClick(TObject *Sender);
    void __fastcall AutoSetupPrmClick(TObject *Sender);
    void __fastcall AccumDataPrmClick(TObject *Sender);

public:
    TDVGParamList(TWinControl *parent, CDevice *device, int PrmCount, const char *str[], int idx);
    ~TDVGParamList();
    void LoadData(byte *memPtr, long memSize, bool HW=false);
    long GetMemLength(){return prmSpaceCnt;};
    void GetPrmMemory(byte *memptr);
    void MemToForm();   // set form from memory
    void FormToMem();   // copy values from form to memory
    long prmSpaceCnt;
    byte *prmSpace;     //memory for all params
};



#endif
