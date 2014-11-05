#include <INIfiles.hpp>
#include "main.h"

extern CDevice *dev;
//---------------------------------------------------------------------------
bool __fastcall TMainForm::ReadIni(void)
{
    int val;
    AnsiString str = ChangeFileExt(Application->ExeName, ".ini" );
    if(str.Length()>1){
        TIniFile *ini = new TIniFile(str);
        val = ini->ReadInteger( "Form", "ComPort", 1);
        if(val < SerialPortListCb1->Items->Count){
            SerialPortListCb1->ItemIndex = val;
             if(SerialPortListCb1->ItemIndex>-1){
                StatusBar1->Panels->Items[0]->Text = "Port: "+SerialPortListCb1->Text;
            } else {
                StatusBar1->Panels->Items[0]->Text = "Port: Not found";
            }
            //SerialNameLabel1->Caption = SerialPortListCb1->Items->Strings[val];
        }

        val = ini->ReadInteger( "Form", "BaudRate", 1);
        delete ini; ini=NULL;
        return true;
    }
    return  false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RefreshPortList()
{
    // refresh por list
    vector<int>portList;
    if(dev->GetAvailablePortList(&portList)>0){
        SerialPortListCb1->Clear();

        for(std::vector<int>::const_iterator it = portList.begin(); it != portList.end(); ++it)
            SerialPortListCb1->Items->Add("COM"+IntToStr((*it)));

        SerialPortListCb1->ItemIndex = 0;
        StatusBar1->Panels->Items[0]->Text = "Port: "+SerialPortListCb1->Text;
        portList.clear();
    } else {
        StatusBar1->Panels->Items[0]->Text = "Port: Not found";
    }
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::SaveIni(void)
{
    AnsiString str = ChangeFileExt(Application->ExeName, ".ini" );
    if(str.Length()>1){
        TIniFile *ini = new TIniFile(str);
        ini->WriteInteger( "Form", "ComPort", SerialPortListCb1->ItemIndex);
        delete ini; ini=NULL;
        return true;
    }
    return  false;
}
//---------------------------------------------------------------------------
long filesize(FILE *stream)
{
   long curpos, length;
   curpos = ftell(stream);
   fseek(stream, 0L, SEEK_END);
   length = ftell(stream);
   fseek(stream, curpos, SEEK_SET);
   return length;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::LoadDriveParams(const char *filename, byte **mem, int *count)
{
    FILE *fl;
    *mem = NULL;
    if ((fl = fopen(filename, "rb")) == NULL) {
        ShowMessage("Cannot open input file");
        return false;
    }
    *count = filesize(fl);
    rewind(fl);
    byte *tmem = (byte*)calloc(*count, 1);
    fread(tmem, *count, sizeof(char), fl);
    fclose(fl);
    *mem = tmem;
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::SaveDriveParams(const char *filename, byte *mem, int count)
{
    FILE *fl;
    if ((fl = fopen(filename, "wb")) == NULL) {
        ShowMessage("Cannot open input file");
        return false;
    }
    fwrite(mem, count, sizeof(char), fl);
    fclose(fl);
    return true;
}
//---------------------------------------------------------------------------
