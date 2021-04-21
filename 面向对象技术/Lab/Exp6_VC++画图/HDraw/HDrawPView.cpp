// HDrawPView.cpp : implementation file
//

#include "stdafx.h"
#include "HDraw.h"
#include "HDrawPView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHDrawPView

IMPLEMENT_DYNCREATE(CHDrawPView, CScrollView)
CHDrawPView::CHDrawPView()
{
		
}

CHDrawPView::~CHDrawPView()
{
}


BEGIN_MESSAGE_MAP(CHDrawPView, CScrollView)
	//{{AFX_MSG_MAP(CHDrawPView)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHDrawPView drawing

void CHDrawPView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	m_tracker.m_rect.SetRect(0,0,GetDocument()->m_cavasW, GetDocument()->m_cavasH);
	m_drawView->MoveWindow(m_tracker.m_rect);
}

void CHDrawPView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
	m_tracker.Draw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CHDrawPView diagnostics

#ifdef _DEBUG
void CHDrawPView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CHDrawPView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

CHDrawDoc* CHDrawPView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHDrawDoc)));
	return (CHDrawDoc*)m_pDocument;
}
/////////////////////////////////////////////////////////////////////////////
// CHDrawPView message handlers

int CHDrawPView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	
	m_tracker.m_rect.SetRect(0,0,GetDocument()->m_cavasW, GetDocument()->m_cavasH); 
	m_tracker.m_nStyle=CRectTracker::resizeOutside; 

	//*here must be pointer, if variable, crashed

	m_drawView = new CHDrawView();
	// TODO: Add your specialized creation code here
	if (!m_drawView->CreateEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR,
		AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,LoadCursor(NULL,IDC_CROSS),
			(HBRUSH)GetStockObject(WHITE_BRUSH),NULL),
		"",WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		m_tracker.m_rect.left,m_tracker.m_rect.top,
		m_tracker.m_rect.right-1,m_tracker.m_rect.bottom-1,
		this->m_hWnd,NULL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_drawView->SetDocument((CHDrawDoc*)m_pDocument);
	m_drawView->ShowWindow(SW_NORMAL);
	m_drawView->UpdateWindow();
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,(long)GetStockObject(GRAY_BRUSH));

	return 0;
}



void CHDrawPView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//in case tracker fail when scrool
	CDC* pDC = GetDC();//获得DC
	OnPrepareDC(pDC);//设置DC的滚动属性，与ScollView的滚动有关
	pDC->DPtoLP(&point);//转换当前点为逻辑位置坐标

	int hit = m_tracker.HitTest(point);
	switch(hit){
	case 2:
	case 5:
	case 6:
		if(m_tracker.Track(this,point)){
			//step1. cavas reset
			GetDocument()->m_cavasH = m_tracker.m_rect.bottom;
			GetDocument()->m_cavasW = m_tracker.m_rect.right;
			//step2. scroll or not
			CRect clientRect;
			GetClientRect(&clientRect);
			SetScrollSizes(MM_TEXT, CSize(m_tracker.m_rect.Width()+10, m_tracker.m_rect.Height()+10));
			
			m_drawView->MoveWindow(m_tracker.m_rect.left, m_tracker.m_rect.top,
				m_tracker.m_rect.right,m_tracker.m_rect.bottom);
			GetDocument()->BackUp();//备份
			Invalidate();
		}
	}
	ReleaseDC(pDC);

	CScrollView::OnLButtonDown(nFlags, point);
}

BOOL CHDrawPView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if(pWnd == this){
		CPoint point;
		//Step1. get cursor position
		GetCursorPos(&point);
		//Step2. convert point from screen to client
		ScreenToClient(&point);

		CDC* pDC = GetDC();//获得DC
		OnPrepareDC(pDC);//设置DC的滚动属性，与ScollView的滚动有关
		pDC->DPtoLP(&point);//转换当前点为逻辑位置坐标

		if(m_tracker.HitTest(point) >= 0){
			//Step3. set cursor, **notice, use nHitTest instead of return of tracker
			m_tracker.SetCursor(pWnd, nHitTest);			
			return true;
		}

		ReleaseDC(pDC);
	}
	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}


void CHDrawPView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	::PostMessage(((CMainFrame *)(AfxGetApp( )->m_pMainWnd))->m_hWnd,1234,(WPARAM)m_drawView,0);
	return ;
}
