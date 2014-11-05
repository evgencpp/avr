object AccumForm: TAccumForm
  Left = 355
  Top = 367
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Accumulate data'
  ClientHeight = 333
  ClientWidth = 173
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 12
    Top = 12
    Width = 32
    Height = 13
    Caption = 'Label1'
  end
  object Label2: TLabel
    Left = 12
    Top = 36
    Width = 60
    Height = 13
    Caption = 'Interval, [ms]'
  end
  object Label3: TLabel
    Left = 12
    Top = 60
    Width = 48
    Height = 13
    Caption = 'Time, [ms]'
  end
  object Label4: TLabel
    Left = 12
    Top = 84
    Width = 45
    Height = 13
    Caption = 'File name'
  end
  object Label5: TLabel
    Left = 12
    Top = 112
    Width = 45
    Height = 13
    Caption = 'Start step'
  end
  object Label6: TLabel
    Left = 12
    Top = 136
    Width = 45
    Height = 13
    Caption = 'Stop step'
  end
  object Label7: TLabel
    Left = 12
    Top = 160
    Width = 38
    Height = 13
    Caption = 'Inc step'
  end
  object SaveBtn: TButton
    Left = 88
    Top = 236
    Width = 75
    Height = 25
    Caption = 'Save'
    TabOrder = 0
    OnClick = SaveBtnClick
  end
  object Edit1: TEdit
    Left = 76
    Top = 32
    Width = 89
    Height = 21
    TabOrder = 1
    Text = '0.025'
  end
  object StartBtn: TButton
    Left = 88
    Top = 268
    Width = 75
    Height = 25
    Caption = 'Start'
    TabOrder = 2
    OnClick = StartBtnClick
  end
  object StopBtn: TButton
    Left = 88
    Top = 300
    Width = 75
    Height = 25
    Caption = 'Stop'
    TabOrder = 3
    OnClick = StopBtnClick
  end
  object Edit2: TEdit
    Left = 76
    Top = 56
    Width = 89
    Height = 21
    TabOrder = 4
    Text = '10'
  end
  object cb1: TCheckBox
    Left = 12
    Top = 184
    Width = 97
    Height = 17
    Caption = 'Save timestamp'
    TabOrder = 5
  end
  object rb1: TRadioButton
    Left = 12
    Top = 208
    Width = 61
    Height = 17
    Caption = 'ASCII'
    Checked = True
    TabOrder = 6
    TabStop = True
  end
  object rb2: TRadioButton
    Left = 88
    Top = 208
    Width = 61
    Height = 17
    Caption = 'Bin'
    TabOrder = 7
  end
  object Edit3: TEdit
    Left = 76
    Top = 80
    Width = 89
    Height = 21
    TabOrder = 8
  end
  object Edit4: TEdit
    Left = 76
    Top = 108
    Width = 89
    Height = 21
    TabOrder = 9
    Text = '80'
  end
  object Edit5: TEdit
    Left = 76
    Top = 132
    Width = 89
    Height = 21
    TabOrder = 10
    Text = '80'
  end
  object Edit6: TEdit
    Left = 76
    Top = 156
    Width = 89
    Height = 21
    TabOrder = 11
    Text = '1'
  end
  object sd: TSaveDialog
    DefaultExt = '*.txt'
    Filter = 'ASCII files (*.txt)|*.txt|Binary files (*.bin)|*.bin'
    Left = 132
    Top = 12
  end
end
