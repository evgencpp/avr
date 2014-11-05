//---------------------------------------------------------------------------

#ifndef TDVGParamH
#define TDVGParamH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include "hdevice.h"
//---------------------------------------------------------------------------
typedef unsigned char byte;
enum dataType_t{dtNone, dtByte, dtWord, dtDWord, dtFloat};
enum dataEdit_t{deNone, deEdit, deDrop, deRadio, deSep};


class TDVGParam
{

public:
    TDVGParam(TWinControl *parent, const char *iString, int TopPos);
    ~TDVGParam();
////////
    byte GetByte(void){
        return (byte)(*ptr);
    }
    void SetByte(byte value){
        *ptr = value;
    }

    WORD GetWord(void){
        union u_tag {byte b[4]; WORD fval;} u;
        u.b[0] = *ptr;
        u.b[1] = *(ptr+1);
        return (WORD)u.fval;
    }

    void SetWord(WORD value){
        union u_tag {byte b[4]; WORD fval;} u;
        u.fval = value;
        *(ptr)   = u.b[0];
        *(ptr+1) = u.b[1];
    }

    DWORD GetDWord(void){
        union u_tag {byte b[4]; DWORD fval;} u;
        u.b[0] = *ptr;
        u.b[1] = *(ptr+1);
        u.b[2] = *(ptr+2);
        u.b[3] = *(ptr+3);
        return (DWORD)u.fval;
    }

    void SetDWord(DWORD value){
        union u_tag {byte b[4]; DWORD fval;} u;
        u.fval = value;
        *(ptr)   = u.b[0];
        *(ptr+1) = u.b[1];
        *(ptr+2) = u.b[2];
        *(ptr+3) = u.b[3];
    }
             /*
    float GetFloat(){
        union u_tag {byte b[4]; float fval;}u;
        u.b[0] = *ptr;
        u.b[1] = *(ptr+1);
        u.b[2] = *(ptr+2);
        u.b[3] = *(ptr+3);
        return (float)u.fval;
    }
    void SetFloat(float value){
        union u_tag {byte b[4]; float fval;}u;
        u.fval = value;
        *(ptr)   = u.b[0];
        *(ptr+1) = u.b[1];
        *(ptr+2) = u.b[2];
        *(ptr+3) = u.b[3];
    }          */
    double GetDouble(){
        union u_tag {byte b[8]; double fval;}u;
        u.b[0] = *ptr;
        u.b[1] = *(ptr+1);
        u.b[2] = *(ptr+2);
        u.b[3] = *(ptr+3);
        u.b[4] = *(ptr+4);
        u.b[5] = *(ptr+5);
        u.b[6] = *(ptr+6);
        u.b[7] = *(ptr+7);
        return (double)u.fval;
    }
    void SetDouble(double value){
        union u_tag {byte b[8]; double fval;}u;
        u.fval = value;
        *(ptr)   = u.b[0];
        *(ptr+1) = u.b[1];
        *(ptr+2) = u.b[2];
        *(ptr+3) = u.b[3];
        *(ptr+4) = u.b[4];
        *(ptr+5) = u.b[5];
        *(ptr+6) = u.b[6];
        *(ptr+7) = u.b[7];
    }
    dataType_t GetType(){return pType;};
    bool isHWPrm(){return useHW;};

    int GetPrmLength(){return pLen;}
    int GetHeight(){return 20;}
    void SetMemPtr(byte *memptr){
        ptr = memptr;
    };
    void Mem2Form(void);    // copy params from interanl memory to form
    void Form2Mem(void);    // copy params from form to interanl memory
    void UpdateFromEdit(void);
    void UpdateToEdit(void);
    void SetEnable(bool enable);
    void SetEnable(bool enable, bool AutoSetup);

protected:
    bool useCaption;
    bool useAutoSetup;
    byte *ptr;
    dataType_t pType;
    dataEdit_t pEditType;
    int pLen;
    int mOptionsCnt;
    bool useCalc;       // using param in calculations
    bool useHW;         // use param in HW connection
    char pName[255];
    char **mOprArray;

    TLabel *prmCaption;
    TBevel *prmBevel;
    TEdit *prmEdit;
    TComboBox *prmCb;
    TRadioButton **prmRadio;


};


#endif
