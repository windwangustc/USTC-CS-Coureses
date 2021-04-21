// HColorBar.cpp : implementation file
// Download by http://www.codefans.net

#include "stdafx.h"
#include "HDraw.h"
#include "HColorBar.h"
#include "HDrawDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HColorBar

HColorBar::HColorBar()
{
	//initiate colors
	int index = 0;
	for(int i = 0; i <= 256; i += 128){
		for(int j = 0; j <= 256; j += 128){
			for(int k = 0; k <= 256; k +=128){
				m_COLOR[index ++] = RGB(256==i?255:i, 256==j?255:j, 256==k?255:k);
			}
		}
	}
	m_COLOR[index] = RGB(1,2,3);
}


HColorBar::~HColorBar()
{
}


BEGIN_MESSAGE_MAP(HColorBar, CToolBar)
	//{{AFX_MSG_MAP(HColorBar)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HColorBar message handlers


COLORREF HColorBar::GetColor()
{
	return m_currentColor;
}

void HColorBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect rc;
	GetClientRect(&rc);
	//step1.1 draw color indicator frame 
	dc.Draw3dRect(rc.left+1, rc.top+5, 33, 33, RGB(0, 0, 0), RGB(255, 255, 255));
	//step1.2 draw color indicator
	CPen pen2(PS_SOLID, 1, m_currentColor);
	CBrush bru2(m_currentColor);
	dc.SelectObject(&pen2);
	dc.SelectObject(&bru2);
	dc.Draw3dRect(rc.left+5, rc.top+5+5, 15, 15, RGB(255, 255, 255), RGB(150, 150, 150));
	dc.Rectangle(rc.left+6, rc.top+10, rc.left+29, rc.top+33);

	//step2.1
	//initiate color rects
    for(int i=0;i<COLORNUMBER/2;i++)
	{
        m_rect[i*2].left=rc.left+36+i*(15+2)+1;
		m_rect[i*2].top=rc.top+5+1;
		m_rect[i*2].right=m_rect[i*2].left+13;
		m_rect[i*2].bottom=m_rect[i*2].top+13;
		
        m_rect[i*2+1].left=rc.left+36+i*(15+2)+1;
		m_rect[i*2+1].top=rc.top+22+1;
		m_rect[i*2+1].right=m_rect[i*2+1].left+13;
		m_rect[i*2+1].bottom=m_rect[i*2+1].top+13;
	};
	//step2.2
	//draw rects
    for(i =0;i<COLORNUMBER/2;i++)
	{
		dc.Draw3dRect(rc.left+36+i*(15+2), rc.top+5, 15, 15, RGB(0, 0, 0), RGB(255, 255, 255));
	    CPen pen3(PS_SOLID, 1, m_COLOR[i*2]);
	    CBrush bru3(m_COLOR[i*2]);
	    dc.SelectObject(&pen3);
	    dc.SelectObject(&bru3);
	    dc.Rectangle(m_rect[i*2].left, m_rect[i*2].top, m_rect[i*2].right, m_rect[i*2].bottom);
		dc.Draw3dRect(rc.left+36+i*(15+2), rc.top+22, 15, 15, RGB(0, 0, 0), RGB(255, 255, 255));
	    CPen pen4(PS_SOLID, 1, m_COLOR[i*2+1]);
	    CBrush bru4(m_COLOR[i*2+1]);
	    dc.SelectObject(&pen4);
	    dc.SelectObject(&bru4);
	    dc.Rectangle(m_rect[i*2+1].left, m_rect[i*2+1].top, m_rect[i*2+1].right, m_rect[i*2+1].bottom);
	};
	//step3 calculate total area
	m_rectAll.left=m_rect[0].left;
	m_rectAll.top=m_rect[0].top;
	m_rectAll.right=m_rect[COLORNUMBER-1].right;
	m_rectAll.bottom=m_rect[1].bottom;
}

void HColorBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CClientDC dc(this);
	CRect rc;
	GetClientRect(&rc);
	for(int i=0;i<COLORNUMBER;i++)
	{
		if(m_rect[i].PtInRect(point))
		{
			//step1. change variable
			m_currentColor = m_COLOR[i];
			//step2. change document
			UpdateDocument();
         	CPen pen(PS_SOLID, 1, m_currentColor);
         	CBrush bru(m_currentColor);
          	dc.SelectObject(&pen);
         	dc.SelectObject(&bru);
			//step2. draw indicator
          	dc.Rectangle(rc.left+6, rc.top+10, rc.left+29, rc.top+33);
			break;
		}
	}
	CToolBar::OnLButtonDown(nFlags, point);
}



void HColorBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CClientDC dc(this);
	CRect rc;
	GetClientRect(&rc);
    if(m_rectAll.PtInRect(point))
	{
        CColorDialog dlg;
		dlg.m_cc.Flags |= CC_RGBINIT;
		dlg.m_cc.rgbResult=m_currentColor;
		if(dlg.DoModal()==IDOK)
		{
			//step1. set variable
			m_currentColor=dlg.GetColor();
			//step2. update document
			UpdateDocument();
         	CPen pen(PS_SOLID, 1, m_currentColor);
			CBrush bru(m_currentColor);
          	dc.SelectObject(&pen);
         	dc.SelectObject(&bru);
			//step3. draw indicator
          	dc.Rectangle(rc.left+6, rc.top+10, rc.left+19, rc.top+24);
		};
	}
}

void HColorBar::UpdateDocument()
{
	//Get Document
	CHDrawDoc* pDoc = ((CHDrawDoc*)(((CMainFrame*)AfxGetMainWnd())->GetActiveDocument()));

	//Set color of document
	pDoc->m_picColor = m_currentColor;

	//Set color of selected strokes
	bool refresh = false;
	for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++){
		if(pDoc->m_strokeList.GetAt(i)->m_bSelected)
			pDoc->m_strokeList.GetAt(i)->m_penColor = m_currentColor;
		refresh = true;
	}
	if(refresh){
		pDoc->BackUp();
		((CMainFrame*)AfxGetMainWnd())->GetActiveView()->Invalidate();
	}
}
