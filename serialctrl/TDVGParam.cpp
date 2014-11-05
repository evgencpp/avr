//---------------------------------------------------------------------------
#pragma hdrstop
#include "TDVGParam.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
bool parseName(char *src, char *dest)
{
    char *fptr1, *fptr2;
    if(((fptr1 = strstr(src, "n=")) != NULL) &&
        (fptr2 = strstr(fptr1, ";"))!=NULL){
            memcpy(dest,(fptr1+2), (fptr2-fptr1 - 2));
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool parseType(char *src, dataType_t *dataType, int *len)
{
    char *fptr1, *fptr2, tmp[255];
    memset(tmp, 0, 255);
    if(((fptr1 = strstr(src, "t=")) != NULL) &&
        (fptr2 = strstr(fptr1, ";"))!=NULL){
        memcpy(tmp,(fptr1+2), (fptr2-fptr1 - 2));

        if(strstr(tmp,"byte")){
            *dataType = dtByte;
            *len = 1;
        } else if(strstr(tmp,"word")){
            *dataType = dtWord;
            *len = 2;
        } else if(strstr(tmp,"dword")){
            *dataType = dtDWord;
            *len = 4;
        } else if (strstr(tmp,"float")){
            *dataType = dtFloat;
            *len = 8;
        } else {
            *dataType = dtNone;
            *len = 0;
        }
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool parseCaption(char *src, bool *Caption)
{
    char *fptr1, *fptr2, tmp[255];
    memset(tmp, 0, 255);
    if(((fptr1 = strstr(src, "c=")) != NULL) &&
        (fptr2 = strstr(fptr1, ";"))!=NULL){
            memcpy(tmp,(fptr1+2), (fptr2-fptr1 - 2));
            if(strstr(tmp,"1"))
                *Caption = true;
            else
                *Caption = false;
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool parseAutoSetup(char *src, bool *Caption)
{
    char *fptr1, *fptr2, tmp[255];
    memset(tmp, 0, 255);
    if(((fptr1 = strstr(src, "a=")) != NULL) &&
        (fptr2 = strstr(fptr1, ";"))!=NULL){
            memcpy(tmp,(fptr1+2), (fptr2-fptr1 - 2));
            if(strstr(tmp,"1"))
                *Caption = true;
            else
                *Caption = false;
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool parseHWUsing(char *src, bool *HWuse)
{
    char *fptr1, *fptr2, tmp[255];
    memset(tmp, 0, 255);
    if(((fptr1 = strstr(src, "hw=")) != NULL) &&
        (fptr2 = strstr(fptr1, ";"))!=NULL){
            memcpy(tmp,(fptr1+2), (fptr2-fptr1 - 2));
            if(strstr(tmp,"1"))
                *HWuse = true;
            else
                *HWuse = false;
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
int CountLetters(char *ptr, char letter)
{
    int cnt=0;
    while(*ptr){
        if(letter==*ptr)
            cnt++;
        ptr++;
    }
    return cnt;
}
//---------------------------------------------------------------------------
bool parseEditType(char *src, dataEdit_t *editType, int *OptionsCnt, char ***options)
{
    char **topt=NULL;
    char *fptr1, *fptr2, tmp[255], *tptr;
    char *sptr, *eptr;
    memset(tmp, 0, 255);
    int optCnt;
    int tcnt, tlen;
    *OptionsCnt = 0;
    *options = NULL;
    if(((fptr1 = strstr(src, "e=")) != NULL) &&
        (fptr2 = strstr(fptr1, ";"))!=NULL){
        memcpy(tmp,(fptr1+2), (fptr2-fptr1 - 2));
        if(strstr(tmp,"edit")){
            *editType = deEdit;
        } else if(strstr(tmp,"delimiter")){
            *editType = deSep;
        }
        else if((tptr = strstr(tmp,"dropdown")) != NULL){
            optCnt = CountLetters(tmp,',');
            *OptionsCnt = optCnt + 1;
            *editType = deDrop;
            fptr1 = tptr+5;
            fptr2 = fptr1;
            tcnt = 0;
            //topt = (char**)new (char*)[*OptionsCnt];
            topt = (char **) calloc(*OptionsCnt, sizeof(char*));
            while(tcnt != *OptionsCnt){
                // find start
                while(*fptr2 != '\''){ fptr2++; }
                sptr = ++fptr2;
                // find end
                while(*fptr2 != '\''){ fptr2++; }
                eptr = fptr2++;
                tlen = eptr-sptr;
                topt[tcnt] = (char *) calloc(255, 1);
                memset((char*)&(topt[tcnt])[0], 0, 255);
                memcpy((char*)&(topt[tcnt])[0], sptr, tlen);
                tcnt++;
            }
            *options = topt;
        }
        else if((tptr = strstr(tmp,"radio")) != NULL){
            optCnt = CountLetters(tmp,',');
            *OptionsCnt = optCnt + 1;
            *editType = deRadio;
            fptr1 = tptr+5;
            fptr2 = fptr1;
            tcnt = 0;
            //topt = (char**)new (char*)[*OptionsCnt];
            topt = (char **) calloc(*OptionsCnt, sizeof(char*));
            while(tcnt != *OptionsCnt){
                // find start
                while(*fptr2 != '\''){ fptr2++; }
                sptr = ++fptr2;
                // find end
                while(*fptr2 != '\''){ fptr2++; }
                eptr = fptr2++;
                tlen = eptr-sptr;
                topt[tcnt] = (char *) calloc(255, 1);
                memset((char*)&(topt[tcnt])[0], 0, 255);
                memcpy((char*)&(topt[tcnt])[0], sptr, tlen);
                tcnt++;
            }
            *options = topt;
        }
        else
            *editType = deNone;


        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
TDVGParam::TDVGParam(TWinControl *parent, const char *iString, int TopPos)
{
    char* dptr = const_cast<char *>(iString);
    memset(pName, 0, 255);
    parseName(dptr, pName);
    parseType(dptr, &pType, &pLen);
    parseCaption(dptr, &useCaption);
    parseAutoSetup(dptr, &useAutoSetup);
    parseHWUsing(dptr, &useHW);
    parseEditType(dptr, &pEditType, &mOptionsCnt, &mOprArray);

    //make interface
    int lft = 10;
    if(useCaption){
        prmCaption =  new TLabel(parent);
        prmCaption->Parent = parent;
        prmCaption->Left = lft;
        prmCaption->Top = TopPos+2;
        prmCaption->Caption = pName;
        lft += 80;
    } else prmCaption = NULL;

    if(pEditType == deEdit){
        prmEdit = new TEdit(parent);
        prmEdit->Parent = parent;
        prmEdit->Left = lft;
        prmEdit->Top = TopPos;
        prmEdit->Text = "0";
        prmEdit->Width = 120;
    } else prmEdit = NULL;

    if(pEditType == deSep){
        prmBevel = new TBevel(parent);
        prmBevel->Parent = parent;
        prmBevel->Left = 0;
        prmBevel->Top = TopPos;
        prmBevel->Width = parent->Width;
        prmBevel->Height = 2;
    } else prmBevel = NULL;

    if(pEditType == deRadio){
        prmRadio = (TRadioButton **)calloc(mOptionsCnt, sizeof(TRadioButton *));
        for(int i=0; i<mOptionsCnt; i++){
            prmRadio[i] = new TRadioButton(parent);
            prmRadio[i]->Parent = parent;
            prmRadio[i]->Top = TopPos;
            prmRadio[i]->Left = lft;
            prmRadio[i]->Width = strlen(mOprArray[i])*9 + 20;
            prmRadio[i]->Caption = mOprArray[i];
            lft+=prmRadio[i]->Width + 10;
        }
    } else prmRadio = NULL;

    if(pEditType == deDrop){
        prmCb = new TComboBox(parent);
        prmCb->Parent = parent;
        prmCb->Top = TopPos;
        prmCb->Left = lft;
        prmCb->Width = 70;
        prmCb->Style = csDropDownList;
        for(int i=0; i<mOptionsCnt; i++)
            prmCb->Items->Add(mOprArray[i]);
    } else prmCb = NULL;

}
//---------------------------------------------------------------------------
TDVGParam::~TDVGParam()
{
    if(prmRadio!=NULL){
        for(int i=0;i<mOptionsCnt;i++){
            delete prmRadio[i]; prmRadio[i] = NULL;
        }
        free(prmRadio); prmRadio = NULL;
    }
    if(prmEdit != NULL){
        delete prmEdit; prmEdit = NULL;
    }
    if(prmCaption != NULL){
        delete prmCaption; prmCaption = NULL;
    }
    if(prmBevel != NULL){
        delete prmBevel; prmBevel = NULL;
    }
    if(prmCb != NULL){
        delete prmCb; prmCb = NULL;
    }
    if(mOprArray){
        for(int i=0;i<mOptionsCnt;i++){
            free(mOprArray[i]);
            mOprArray[i] = NULL;
        }
        free(mOprArray); mOprArray = NULL;
    }
}
//---------------------------------------------------------------------------
void TDVGParam::UpdateFromEdit()
{
    // set memory from edit
    int a0;
    double a1;
    switch(pType){
        case dtByte:
            if(TryStrToInt(prmEdit->Text, a0))  SetByte((byte)a0);
            else SetByte(0);
            break;
        case dtWord:
            if(TryStrToInt(prmEdit->Text, a0)) SetWord((WORD)a0);
            else SetWord(0);
            break;
        case dtDWord:
            if(TryStrToInt(prmEdit->Text, a0)) SetDWord((DWORD) a0);
            else SetDWord(0);
            break;
        case dtFloat:
            if(TryStrToFloat(prmEdit->Text, a1)) SetDouble(a1);
            else SetDouble(0);
            break;
        default:
            break;
    }
}
//---------------------------------------------------------------------------
void TDVGParam::UpdateToEdit()
{
    // set edit from memory
    switch(pType){
        case dtByte:
            prmEdit->Text = IntToStr(GetByte());
            break;
        case dtWord:
            prmEdit->Text = IntToStr(GetWord());
            break;
        case dtDWord:
            prmEdit->Text = IntToStr(GetDWord());
            break;
        case dtFloat:
            prmEdit->Text = FloatToStr(GetDouble());
            break;
        default:
            break;
    }
}
//---------------------------------------------------------------------------
void TDVGParam::Mem2Form(void)
{
    // copy params from in  teranl memory to form
    byte ff;
    switch(pEditType){
        case deEdit:
            if(deEdit)
                UpdateToEdit();
            break;
        case deDrop:
            if(prmCb)
            prmCb->ItemIndex = GetByte();
            break;
        case deRadio:
            ff = GetByte();
            if(ff < mOptionsCnt)
                prmRadio[ff]->Checked = true;
            break;
        default: break;
    }
}
//---------------------------------------------------------------------------
void TDVGParam::Form2Mem(void)
{
    int i;
    // copy params from form to interanl memory
    switch(pEditType){
        case deEdit:
            UpdateFromEdit();
            break;
        case deDrop:
            SetByte(prmCb->ItemIndex);
            break;
        case deRadio:
            for(i=0;i<mOptionsCnt; i++)
                if(prmRadio[i]->Checked)
                    break;
            SetByte(i);
            break;
        default: break;
    }
}
//---------------------------------------------------------------------------
void TDVGParam::SetEnable(bool enable, bool AutoSetup)
{
    switch(pEditType){
        case deEdit:
            if(AutoSetup) prmEdit->Enabled = (useAutoSetup)?enable:true;
            else prmEdit->Enabled = enable;
            break;
        case deDrop:
            if(AutoSetup) prmCb->Enabled = (useAutoSetup)?enable:true;
            else prmCb->Enabled = enable;
            break;
        case deRadio:
            for(int i=0;i<mOptionsCnt; i++)
                if(AutoSetup) prmRadio[i]->Enabled = (useAutoSetup)?enable:true;
                else prmRadio[i]->Enabled = enable;
            break;
        default: break;
    }
}
//---------------------------------------------------------------------------
