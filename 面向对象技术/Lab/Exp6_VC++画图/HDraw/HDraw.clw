; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CHDrawView
LastTemplate=CToolBarCtrl
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "HDraw.h"
LastPage=0

ClassCount=11
Class1=CHDrawApp
Class2=CHDrawDoc
Class3=CHDrawView
Class4=CMainFrame

ResourceCount=7
Resource1=IDD_DLG_SET
Class5=CAboutDlg
Resource2=IDR_TOOLBAR_COLOR
Class6=HStrokeEditDlg
Resource3=IDR_MAINFRAME
Class7=HStrokeTextDlg
Class8=HColorButton
Class9=CHDrawPView
Class10=HColorBar
Resource4=IDR_MENU_SET
Class11=HWidthBar
Resource5=IDR_TOOLBAR_PIC
Resource6=IDD_ABOUTBOX
Resource7=IDD_DLG_TEXT

[CLS:CHDrawApp]
Type=0
HeaderFile=HDraw.h
ImplementationFile=HDraw.cpp
Filter=N

[CLS:CHDrawDoc]
Type=0
HeaderFile=HDrawDoc.h
ImplementationFile=HDrawDoc.cpp
Filter=N
BaseClass=CDocument
VirtualFilter=DC
LastObject=CHDrawDoc

[CLS:CHDrawView]
Type=0
HeaderFile=HDrawView.h
ImplementationFile=HDrawView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=ID_DRAW_CLEAR


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CFrameWnd
VirtualFilter=fWC
LastObject=ID_BMP_SAVE




[CLS:CAboutDlg]
Type=0
HeaderFile=HDraw.cpp
ImplementationFile=HDraw.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_BMP_SAVE
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_REDO
Command13=ID_EDIT_CUT
Command14=ID_EDIT_COPY
Command15=ID_EDIT_PASTE
Command16=ID_VIEW_TOOLBAR
Command17=ID_VIEW_STATUS_BAR
Command18=ID_VIEW_COLOR
Command19=ID_VIEW_PICS
Command20=ID_DRAW_LINE
Command21=ID_DRAW_POLY
Command22=ID_DRAW_RECT
Command23=ID_DRAW_ELLIPSE
Command24=ID_DRAW_TEXT
Command25=ID_PEN_SOLID
Command26=ID_PEN_DASH
Command27=ID_BT_W1
Command28=ID_BT_W2
Command29=ID_BT_W3
Command30=ID_APP_ABOUT
CommandCount=30

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_EDIT_COPY
Command2=ID_BMP_SAVE
Command3=ID_FILE_NEW
Command4=ID_FILE_OPEN
Command5=ID_FILE_PRINT
Command6=ID_EDIT_REDO
Command7=ID_FILE_SAVE
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_NEXT_PANE
Command12=ID_PREV_PANE
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_EDIT_CUT
Command16=ID_EDIT_UNDO
CommandCount=16

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
Command9=ID_DRAW_CLEAR
Command10=ID_DRAW_SET
CommandCount=10

[DLG:IDD_DLG_SET]
Type=1
Class=HStrokeEditDlg
ControlCount=9
Control1=IDCANCEL,button,1342242816
Control2=IDC_LIST_STROKES,listbox,1352728833
Control3=IDC_BTN_UPDATE,button,1342242816
Control4=IDC_EDIT_STROKE,edit,1350631552
Control5=IDC_BTN_DEL,button,1342242816
Control6=IDC_EDIT_WIDTH,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_BTN_COLOR,button,1342242816
Control9=ID_OK,button,1342242816

[DLG:IDD_DLG_TEXT]
Type=1
Class=HStrokeTextDlg
ControlCount=2
Control1=IDC_EDIT_TEXT,edit,1350631552
Control2=IDOK,button,1342242817

[CLS:HStrokeTextDlg]
Type=0
HeaderFile=HStrokeTextDlg.h
ImplementationFile=HStrokeTextDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_EDIT_TEXT

[CLS:HStrokeEditDlg]
Type=0
HeaderFile=hstrokeeditdlg.h
ImplementationFile=hstrokeeditdlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_LIST_STROKES

[CLS:HColorButton]
Type=0
HeaderFile=HColorButton.h
ImplementationFile=HColorButton.cpp
BaseClass=CButton
Filter=W
VirtualFilter=BWC

[CLS:CHDrawPView]
Type=0
HeaderFile=HDrawPView.h
ImplementationFile=HDrawPView.cpp
BaseClass=CScrollView
Filter=C
VirtualFilter=VWC
LastObject=CHDrawPView

[TB:IDR_TOOLBAR_COLOR]
Type=1
Class=?
Command1=IDB_COLOR
CommandCount=1

[CLS:HWidthBar]
Type=0
HeaderFile=HWidthBar.h
ImplementationFile=HWidthBar.cpp
BaseClass=CToolBarCtrl
Filter=W
LastObject=ID_BUTTON32789

[CLS:HColorBar]
Type=0
HeaderFile=hcolorbar.h
ImplementationFile=hcolorbar.cpp
BaseClass=CToolBar
LastObject=HColorBar

[TB:IDR_TOOLBAR_PIC]
Type=1
Class=?
Command1=ID_SELECT
Command2=ID_DRAW_LINE
Command3=ID_DRAW_POLY
Command4=ID_DRAW_ELLIPSE
Command5=ID_DRAW_RECT
Command6=ID_DRAW_TEXT
Command7=ID_BT_W1
Command8=ID_BT_W2
Command9=ID_BT_W3
Command10=ID_PEN_SOLID
Command11=ID_PEN_DASH
CommandCount=11

[MNU:IDR_MENU_SET]
Type=1
Class=?
Command1=ID_PIC_COLOR_RED
Command2=ID_PIC_COLOR_GREEN
Command3=ID_PIC_COLOR_BLUE
Command4=ID_PIC_COLOR_OTHER
Command5=ID_PIC_SOLID
Command6=ID_PIC_DASH
Command7=ID_PIC_W1
Command8=ID_PIC_W2
Command9=ID_PIC_W3
Command10=ID_PIC_DELETE
Command11=ID_DRAW_SET
CommandCount=11

