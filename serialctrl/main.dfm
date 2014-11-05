object MainForm: TMainForm
  Left = 347
  Top = 133
  Width = 800
  Height = 682
  Caption = 'MainForm'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object pc: TPageControl
    Left = 0
    Top = 25
    Width = 784
    Height = 600
    ActivePage = TabSheet1
    Align = alClient
    TabIndex = 1
    TabOrder = 0
    object SettingsTab1: TTabSheet
      Caption = 'Settings'
      ImageIndex = 1
      object ScrollBox1: TScrollBox
        Left = 0
        Top = 0
        Width = 776
        Height = 572
        VertScrollBar.Visible = False
        Align = alClient
        BorderStyle = bsNone
        TabOrder = 0
      end
    end
    object TabSheet1: TTabSheet
      Caption = 'View Ctrl'
      ImageIndex = 2
      object Splitter1: TSplitter
        Left = 0
        Top = 185
        Width = 776
        Height = 3
        Cursor = crVSplit
        Align = alTop
      end
      object Panel3: TPanel
        Left = 0
        Top = 437
        Width = 776
        Height = 135
        Align = alBottom
        TabOrder = 0
        object CheckListBox1: TCheckListBox
          Left = 1
          Top = 1
          Width = 72
          Height = 133
          Align = alLeft
          ItemHeight = 13
          Items.Strings = (
            'Velocity'
            'Curent'
            'UItem1'
            'UItem2')
          TabOrder = 0
        end
      end
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 776
        Height = 185
        Align = alTop
        TabOrder = 1
        object Chart1: TChart
          Left = 1
          Top = 1
          Width = 774
          Height = 183
          BackWall.Brush.Color = clWhite
          BackWall.Brush.Style = bsClear
          Title.Text.Strings = (
            'Velocity')
          Legend.Visible = False
          View3D = False
          Align = alClient
          TabOrder = 0
          object Series1: TFastLineSeries
            Marks.ArrowLength = 8
            Marks.Visible = False
            SeriesColor = clRed
            ValueFormat = '#,##:0.###'
            LinePen.Color = clRed
            XValues.DateTime = True
            XValues.Name = 'X'
            XValues.Multiplier = 1
            XValues.Order = loAscending
            YValues.DateTime = False
            YValues.Name = 'Y'
            YValues.Multiplier = 1
            YValues.Order = loNone
          end
          object Series5: TFastLineSeries
            Marks.ArrowLength = 8
            Marks.Visible = False
            SeriesColor = clBlue
            LinePen.Color = clBlue
            XValues.DateTime = True
            XValues.Name = 'X'
            XValues.Multiplier = 1
            XValues.Order = loAscending
            YValues.DateTime = False
            YValues.Name = 'Y'
            YValues.Multiplier = 1
            YValues.Order = loNone
          end
        end
      end
      object Panel4: TPanel
        Left = 0
        Top = 188
        Width = 776
        Height = 249
        Align = alClient
        TabOrder = 2
        object Chart3: TChart
          Left = 1
          Top = 1
          Width = 774
          Height = 247
          BackWall.Brush.Color = clWhite
          BackWall.Brush.Style = bsClear
          Title.Text.Strings = (
            'Current sensors')
          Legend.Visible = False
          View3D = False
          Align = alClient
          TabOrder = 0
          object Series2: TFastLineSeries
            Marks.ArrowLength = 8
            Marks.Visible = False
            SeriesColor = clRed
            LinePen.Color = clRed
            XValues.DateTime = True
            XValues.Name = 'X'
            XValues.Multiplier = 1
            XValues.Order = loAscending
            YValues.DateTime = False
            YValues.Name = 'Y'
            YValues.Multiplier = 1
            YValues.Order = loNone
          end
          object Series3: TFastLineSeries
            Marks.ArrowLength = 8
            Marks.Visible = False
            SeriesColor = clBlue
            LinePen.Color = clBlue
            XValues.DateTime = False
            XValues.Name = 'X'
            XValues.Multiplier = 1
            XValues.Order = loAscending
            YValues.DateTime = False
            YValues.Name = 'Y'
            YValues.Multiplier = 1
            YValues.Order = loNone
          end
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Programming'
      ImageIndex = 2
      object Panel5: TPanel
        Left = 591
        Top = 0
        Width = 185
        Height = 572
        Align = alRight
        TabOrder = 0
      end
      object Panel6: TPanel
        Left = 0
        Top = 0
        Width = 591
        Height = 572
        Align = alClient
        BevelOuter = bvNone
        Caption = 'Panel6'
        TabOrder = 1
        object Panel7: TPanel
          Left = 0
          Top = 550
          Width = 591
          Height = 22
          Align = alBottom
          BevelOuter = bvNone
          TabOrder = 0
          object ScriptEdit1: TEdit
            Left = 1
            Top = 1
            Width = 121
            Height = 21
            TabOrder = 0
            Text = 'ScriptEdit1'
          end
          object RunScriptBtn1: TButton
            Left = 575
            Top = 1
            Width = 22
            Height = 21
            Caption = '>>'
            TabOrder = 1
          end
        end
        object Memo1: TMemo
          Left = 0
          Top = 0
          Width = 591
          Height = 550
          Align = alClient
          Lines.Strings = (
            'Memo1')
          TabOrder = 1
        end
      end
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 625
    Width = 784
    Height = 19
    Panels = <
      item
        Text = 'Port:'
        Width = 80
      end
      item
        Text = 'Status: OFFline'
        Width = 90
      end
      item
        Width = 50
      end>
    SimplePanel = False
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 784
    Height = 25
    Align = alTop
    TabOrder = 2
    object OpenPortBtn1: TButton
      Left = 92
      Top = 2
      Width = 75
      Height = 21
      Caption = 'Open'
      TabOrder = 0
      OnClick = OpenPortBtn1Click
    end
    object SerialPortListCb1: TComboBox
      Left = 1
      Top = 3
      Width = 89
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 1
      OnChange = SerialPortListCb1Change
    end
  end
  object PingPongTimer1: TTimer
    Enabled = False
    OnTimer = PingPongTimer1Timer
    Left = 48
    Top = 40
  end
  object SnsTimer1: TTimer
    Enabled = False
    Interval = 100
    OnTimer = SnsTimer1Timer
    Left = 16
    Top = 40
  end
  object UpdateCtrlTimer: TTimer
    Enabled = False
    Interval = 50
    OnTimer = UpdateCtrlTimerTimer
    Left = 16
    Top = 72
  end
end
