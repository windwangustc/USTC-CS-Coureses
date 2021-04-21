// HDrawDoc.cpp : implementation of the CHDrawDoc class
// Download by http://www.codefans.net

#include "stdafx.h"
#include "HDraw.h"

#include "HDrawDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHDrawDoc

IMPLEMENT_DYNCREATE(CHDrawDoc, CDocument)

BEGIN_MESSAGE_MAP(CHDrawDoc, CDocument)
	//{{AFX_MSG_MAP(CHDrawDoc)
	ON_COMMAND(ID_DRAW_CLEAR, OnDrawClear)
	ON_COMMAND(ID_DRAW_ELLIPSE, OnDrawEllipse)
	ON_COMMAND(ID_DRAW_LINE, OnDrawLine)
	ON_COMMAND(ID_DRAW_POLY, OnDrawPoly)
	ON_COMMAND(ID_DRAW_RECT, OnDrawRect)
	ON_COMMAND(ID_DRAW_SET, OnDrawSet)
	ON_COMMAND(ID_DRAW_TEXT, OnDrawText)
	ON_COMMAND(ID_BT_W1, OnBtW1)
	ON_COMMAND(ID_BT_W2, OnBtW2)
	ON_COMMAND(ID_BT_W3, OnBtW3)
	ON_UPDATE_COMMAND_UI(ID_BT_W1, OnUpdateBtW1)
	ON_UPDATE_COMMAND_UI(ID_BT_W2, OnUpdateBtW2)
	ON_UPDATE_COMMAND_UI(ID_BT_W3, OnUpdateBtW3)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ELLIPSE, OnUpdateDrawEllipse)
	ON_UPDATE_COMMAND_UI(ID_DRAW_LINE, OnUpdateDrawLine)
	ON_UPDATE_COMMAND_UI(ID_DRAW_POLY, OnUpdateDrawPoly)
	ON_UPDATE_COMMAND_UI(ID_DRAW_RECT, OnUpdateDrawRect)
	ON_UPDATE_COMMAND_UI(ID_DRAW_TEXT, OnUpdateDrawText)
	ON_COMMAND(ID_PEN_DASH, OnPenDash)
	ON_UPDATE_COMMAND_UI(ID_PEN_DASH, OnUpdatePenDash)
	ON_COMMAND(ID_PEN_SOLID, OnPenSolid)
	ON_UPDATE_COMMAND_UI(ID_PEN_SOLID, OnUpdatePenSolid)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_SELECT, OnSelect)
	ON_UPDATE_COMMAND_UI(ID_SELECT, OnUpdateSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHDrawDoc construction/destruction

CHDrawDoc::CHDrawDoc()
{
	m_picType = PIC_line;
	m_picColor = RGB(0,0,0);
	m_picWidth = 1;
	m_picStyle = PS_SOLID;

	m_cavasH = 300;
	m_cavasW = 600;

	m_backup_index = 0;
	BackUp();
}

CHDrawDoc::~CHDrawDoc()
{
	DeleteContents();
	//backup information
	for(int i = 0; i < m_backup.GetSize(); i ++){
		CFile::Remove(m_backup.GetAt(i));
	}
	m_backup.RemoveAll();
}

BOOL CHDrawDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CHDrawDoc serialization

void CHDrawDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar<<m_cavasH<<m_cavasW;
		m_strokeList.Serialize(ar);
	}
	else
	{
		// TODO: add loading code here
		ar>>m_cavasH>>m_cavasW;
		m_strokeList.Serialize(ar);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHDrawDoc diagnostics

#ifdef _DEBUG
void CHDrawDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHDrawDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


HStroke * CHDrawDoc::NewStroke()
{
	HStroke *p;
	switch(m_picType){
	case PIC_line:
		p = new HStrokeLine();
		break;
	case PIC_poly:
		p = new HStrokePoly();
		break;
	case PIC_ellipse:
		p = new HStrokeEllipse();
		break;
	case PIC_rect:
		p = new HStrokeRect();
		break;
	case PIC_text:
		p = new HStrokeText(m_picInfo);
		break;
	case PIC_select:
		p = new HStrokeSelect();
		break;
	default:
		p = NULL;
	}
	p->m_penColor = m_picColor;
	p->m_penWidth = m_picWidth;
	p->m_penStyle = m_picStyle;
	return p;
}

/////////////////////////////////////////////////////////////////////////////
// CHDrawDoc commands

void CHDrawDoc::OnDrawClear() 
{
	// TODO: Add your command handler code here
	m_strokeList.RemoveAll();
	UpdateAllViews(NULL);
}

void CHDrawDoc::OnDrawEllipse() 
{
	// TODO: Add your command handler code here
	m_picType = PIC_ellipse;
}
void CHDrawDoc::OnDrawLine() 
{
	// TODO: Add your command handler code here
	m_picType = PIC_line;
}

void CHDrawDoc::OnDrawPoly() 
{
	// TODO: Add your command handler code here
	m_picType = PIC_poly;
}

void CHDrawDoc::OnDrawRect() 
{
	// TODO: Add your command handler code here
	m_picType = PIC_rect;
}

//设置图形参数和其它属性
void CHDrawDoc::OnDrawSet() 
{
	// TODO: Add your command handler code here
	HStrokeEditDlg dlg;
	dlg.setStrokeList(&m_strokeList);
	dlg.DoModal();
}

void CHDrawDoc::OnDrawText() 
{
	// TODO: Add your command handler code here
	HStrokeTextDlg dlg;
	if(dlg.DoModal() == IDOK){
		m_picType = PIC_text;
		m_picInfo = dlg.m_editText;
	}
}

void CHDrawDoc::OnBtW1() 
{
	m_picWidth = 1;
	for(int i = 0; i < m_strokeList.GetSize(); i ++){
		if(m_strokeList.GetAt(i)->m_bSelected){
			m_strokeList.GetAt(i)->m_penWidth = m_picWidth;
		}
	}
	UpdateAllViews(NULL);
}

void CHDrawDoc::OnBtW2() 
{
	// TODO: Add your command handler code here
	m_picWidth = 2;
	for(int i = 0; i < m_strokeList.GetSize(); i ++){
		if(m_strokeList.GetAt(i)->m_bSelected){
			m_strokeList.GetAt(i)->m_penWidth = m_picWidth;
		}
	}
	UpdateAllViews(NULL);
}

void CHDrawDoc::OnBtW3() 
{
	// TODO: Add your command handler code here
	m_picWidth = 3;
	for(int i = 0; i < m_strokeList.GetSize(); i ++){
		if(m_strokeList.GetAt(i)->m_bSelected){
			m_strokeList.GetAt(i)->m_penWidth = m_picWidth;
		}
	}
	UpdateAllViews(NULL);
}


void CHDrawDoc::OnUpdateBtW1(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(1 == m_picWidth);
}

void CHDrawDoc::OnUpdateBtW2(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(2 == m_picWidth);
}

void CHDrawDoc::OnUpdateBtW3(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(2 < m_picWidth);
}


void CHDrawDoc::OnUpdateDrawEllipse(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(PIC_ellipse == m_picType);
}

void CHDrawDoc::OnUpdateDrawLine(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(PIC_line == m_picType);
}

void CHDrawDoc::OnUpdateDrawPoly(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(PIC_poly == m_picType);
}

void CHDrawDoc::OnUpdateDrawRect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(PIC_rect == m_picType);
}

void CHDrawDoc::OnUpdateDrawText(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(PIC_text == m_picType);
}

void CHDrawDoc::OnPenDash() 
{
	// TODO: Add your command handler code here
	m_picStyle = PS_DASH;
	for(int i = 0; i < m_strokeList.GetSize(); i ++){
		if(m_strokeList.GetAt(i)->m_bSelected
			||m_strokeList.GetAt(i)->m_bHighLight){
			m_strokeList.GetAt(i)->m_penStyle = m_picStyle;
		}
	}
	BackUp();
	UpdateAllViews(NULL);
}

void CHDrawDoc::OnUpdatePenDash(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(PS_DASH == m_picStyle);
}

void CHDrawDoc::OnPenSolid() 
{
	// TODO: Add your command handler code here
	m_picStyle = PS_SOLID;
	for(int i = 0; i < m_strokeList.GetSize(); i ++){
		if(m_strokeList.GetAt(i)->m_bSelected
			||m_strokeList.GetAt(i)->m_bHighLight){
			m_strokeList.GetAt(i)->m_penStyle = m_picStyle;
		}
	}
	BackUp();
	UpdateAllViews(NULL);
}

void CHDrawDoc::OnUpdatePenSolid(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(PS_SOLID == m_picStyle);
}

void CHDrawDoc::BackUp()
{
	//CArchive cr;
	CString fileName;
	fileName.Format("C:\\Windows\\Temp\\hankjin.temp.%d", m_backup.GetSize());
	OnSaveDocument(fileName);
	m_backup.InsertAt(m_backup_index, fileName);
	m_backup_index ++;
}

void CHDrawDoc::ReStore(BOOL backward)
{
	m_backup_index -= backward ? 1 : -1;
	//make m_backup[m_backup_index] the current document data
	OnOpenDocument(m_backup.GetAt(m_backup_index-1));
}


void CHDrawDoc::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_backup_index < m_backup.GetSize());
}
void CHDrawDoc::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_backup_index > 1);
}



void CHDrawDoc::OnEditRedo() 
{
	// TODO: Add your command handler code here
	ReStore(FALSE);
	UpdateAllViews(NULL);
}

void CHDrawDoc::OnEditUndo() 
{
	// TODO: Add your command handler code here
	ReStore(TRUE);
	UpdateAllViews(NULL);
}

void CHDrawDoc::DeleteContents() 
{
	// TODO: Add your specialized code here and/or call the base class
	//strokeList
	for(int i = 0; i < m_strokeList.GetSize(); i ++)
		delete m_strokeList.ElementAt(i);
	m_strokeList.RemoveAll();

	CDocument::DeleteContents();
}



void CHDrawDoc::OnSelect() 
{
	// TODO: Add your command handler code here
	m_picType = PIC_select;
}

void CHDrawDoc::OnUpdateSelect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(PIC_select == m_picType);
}
