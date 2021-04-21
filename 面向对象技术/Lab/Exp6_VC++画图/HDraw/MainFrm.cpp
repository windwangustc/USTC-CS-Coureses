// MainFrm.cpp : implementation of the CMainFrame class
// Download by http://www.codefans.net

#include "stdafx.h"
#include "HDraw.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_PICS, OnViewPics)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PICS, OnUpdateViewPics)
	ON_COMMAND(ID_VIEW_COLOR, OnViewColor)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COLOR, OnUpdateViewColor)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_ACTIVE, OnOtherViewActive)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_icons[0] = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON1));
	m_icons[1] = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON2));
	m_icons[2] = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON3));
	m_icons[3] = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON4));
	m_icons[4] = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON5));
}

CMainFrame::~CMainFrame()
{
	if(m_colorBar != NULL)
		delete m_colorBar;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	//my picture bar
	if (!m_picBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_LEFT
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED) ||
		!m_picBar.LoadToolBar(IDR_TOOLBAR_PIC))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	int nCount = m_picBar.GetToolBarCtrl().GetButtonCount();
	for(int i=0;i<nCount;i++)
	{
		UINT nStyle=m_picBar.GetButtonStyle(i);
		nStyle|=TBBS_WRAPPED;
		m_picBar.SetButtonStyle(i,nStyle);
	}

	m_colorBar = new HColorBar();
	//my color bar
	if (!m_colorBar->CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_colorBar->LoadToolBar(IDR_TOOLBAR_COLOR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
	m_picBar.EnableDocking(CBRS_ALIGN_LEFT);
	m_colorBar->EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_RIGHT);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_picBar);
	DockControlBar(&m_wndToolBar);
	DockControlBar(m_colorBar);

	SetClassLong(m_hWnd, GCL_HICON, (LONG)m_icons[0]);
	SetTimer(WM_ICONALT, 1000, NULL);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers



LRESULT CMainFrame::OnOtherViewActive(WPARAM wParam,LPARAM lParam)
{
	m_pViewActive = (CView *)wParam;
	return 0;
}

void CMainFrame::OnViewPics() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_picBar, !m_picBar.IsWindowVisible(), FALSE);
	//must use this to make the toolbar disappear
	//if not this command, the toolbar still locate there while components hide
	RecalcLayout();
	//the toolbar goes forever if closed and no the command below
	DockControlBar(&m_picBar);
}

void CMainFrame::OnViewColor() 
{
	// TODO: Add your command handler code here
	//alternate show/hide
	ShowControlBar(m_colorBar, !m_colorBar->IsWindowVisible(), FALSE);
	//must use this to make the toolbar disappear
	//if not this command, the toolbar still locate there while components hide
	RecalcLayout();
	DockControlBar(m_colorBar);
}

void CMainFrame::OnUpdateViewColor(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_colorBar->IsWindowVisible());
}

void CMainFrame::OnUpdateViewPics(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_picBar.IsWindowVisible());
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	static int iconIndex = 1;
	if(nIDEvent == WM_ICONALT){
		SetClassLong(m_hWnd, GCL_HICON, (LONG)m_icons[iconIndex]);
		iconIndex = (++iconIndex) % 5;
	}
	CFrameWnd::OnTimer(nIDEvent);
}
