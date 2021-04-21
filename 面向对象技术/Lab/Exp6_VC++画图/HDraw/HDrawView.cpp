// HDrawView.cpp : implementation of the CHDrawView class
// Download by http://www.codefans.net

#include "stdafx.h"
#include "HDraw.h"

#include "HDrawDoc.h"
#include "HDrawView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHDrawView

BEGIN_MESSAGE_MAP(CHDrawView, CView)
	//{{AFX_MSG_MAP(CHDrawView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_BMP_SAVE, OnBmpSave)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_PIC_COLOR_BLUE, OnPicColorBlue)
	ON_COMMAND(ID_PIC_COLOR_GREEN, OnPicColorGreen)
	ON_COMMAND(ID_PIC_COLOR_OTHER, OnPicColorOther)
	ON_COMMAND(ID_PIC_COLOR_RED, OnPicColorRed)
	ON_COMMAND(ID_PIC_DASH, OnPicDash)
	ON_COMMAND(ID_PIC_SOLID, OnPicSolid)
	ON_COMMAND(ID_PIC_W1, OnPicW1)
	ON_COMMAND(ID_PIC_W2, OnPicW2)
	ON_COMMAND(ID_PIC_W3, OnPicW3)
	ON_COMMAND(ID_PIC_DELETE, OnPicDelete)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHDrawView construction/destruction
CHDrawView::CHDrawView()
{
	// TODO: add construction code here
	m_pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
}

CHDrawView::~CHDrawView()
{
	m_pDocument = pOldDoc;
}

BOOL CHDrawView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CHDrawView drawing

void CHDrawView::OnDraw(CDC* pDC)
{
	CHDrawDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	OnPrepareDC(pDC);

	for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++){
		pDoc->m_strokeList.GetAt(i)->DrawStroke(pDC);
	}
}



/////////////////////////////////////////////////////////////////////////////
// CHDrawView diagnostics

#ifdef _DEBUG
void CHDrawView::AssertValid() const
{
	CView::AssertValid();
}

void CHDrawView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}


CHDrawDoc* CHDrawView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHDrawDoc)));
	return (CHDrawDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHDrawView message handlers
/************************************************************************
很复杂的条件判断
条件1：鼠标是否点中图形：是/否
条件2：是否按下Ctrl：是/否
条件3：选中多少个：一/多/零

非Ctrl+Track：
点中+Ctrl：			选中或取消选中
点中+非Ctrl：		选中点中，取消所有非点中

************************************************************************/
void CHDrawView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CDC* pDC = GetDC();
	OnPrepareDC(pDC);
	pDC->DPtoLP(&point);

	CHDrawDoc* pDoc = GetDocument();
	BOOL refresh = FALSE, bTrack = false, ctrlPressed = nFlags & MK_CONTROL;
	int i;
	//没有Ctrl, Track
	if(!ctrlPressed){
		for(i = 0; i < pDoc->m_strokeList.GetSize(); i ++){
			if(pDoc->m_strokeList.GetAt(i)->m_bSelected){
				int hit = pDoc->m_strokeList.GetAt(i)->m_tracker.HitTest(point);
				if(hit >= 0 &&
					pDoc->m_strokeList.GetAt(i)->m_tracker.Track(this,point)){
					//step1. cavas reset
					pDoc->m_strokeList.GetAt(i)->ReSize(
						pDoc->m_strokeList.GetAt(i)->m_tracker.m_rect);
					Invalidate();
					GetDocument()->BackUp();//备份
					bTrack = true;
					break;
				}
			}
		}
	}
	for(i = 0; i < pDoc->m_strokeList.GetSize(); i ++){
		if(pDoc->m_strokeList.GetAt(i)->IsPointIn(point)){
			//情况1. 点中，Ctrl按下，选中或取消选中
			if(ctrlPressed){
				//select/deselect
				pDoc->m_strokeList.GetAt(i)->m_bSelected = 
					!pDoc->m_strokeList.GetAt(i)->m_bSelected;
			}
			//情况2. 点中，没有Ctrl，选中
			else{
				//select
				pDoc->m_strokeList.GetAt(i)->m_bSelected = true;
			}
			refresh = true;
		}
		//情况3. 没有点中，没有Ctrl，取消选中
		else if(!ctrlPressed)
			pDoc->m_strokeList.GetAt(i)->m_bSelected = false;
	}
	if(refresh){
		Invalidate();
	}
	else{
		//画图
		if(!bTrack && !ctrlPressed){
			//Step1. 设置捕获
			SetCapture();
			//Step2. 加入新图形
			m_stroke = pDoc->NewStroke();
			//Step3. 设置起点
			m_stroke->SetCurrentPoint(point);
			//Step4. 设置文件已经修改状态
			pDoc->SetModifiedFlag();
		}
	}
	
	ReleaseDC(pDC);

	CView::OnLButtonDown(nFlags, point);
}

void CHDrawView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(GetCapture() == this){
		CHDrawDoc *pDoc = GetDocument();

 		CDC *pDC = GetDC();
 		OnPrepareDC(pDC);
 		pDC->DPtoLP(&point);

		//Step0.1 点下松开，没有画
		if(m_stroke->m_points.GetSize() == 1){
			delete m_stroke;
		}
		//Step0.2 选择框
		else if(PIC_select == m_stroke->m_picType){
			bool refresh = false;
			CRect rect(m_stroke->m_points[0], m_stroke->m_points[1]);
			rect.NormalizeRect();//防止从右下往左上选
			for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++){
				if(rect.PtInRect(pDoc->m_strokeList.GetAt(i)->m_points.GetAt(0)) &&
					rect.PtInRect(pDoc->m_strokeList.GetAt(i)->m_points.GetAt(1))){
					pDoc->m_strokeList.GetAt(i)->m_bSelected = true;
					refresh = true;
				}
			}
			if(refresh)
				Invalidate();
			delete m_stroke;
		}
		//Step0.3 画
		else{
			//Step1. 加入新点
			m_stroke->SetCurrentPoint(point);
			pDoc->m_strokeList.Add(m_stroke);
			pDoc->BackUp();//备份
		}
		//Step2. 释放捕获
		ReleaseCapture();
		m_stroke = NULL;
		
 		ReleaseDC(pDC);

		//Step3. 全部重画
		Invalidate();
	}	
	CView::OnLButtonUp(nFlags, point);
}

void CHDrawView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CString curPos;
	curPos.Format("(%d,%d)", point.x, point.y);
	GetParentFrame()->SetMessageText(curPos);

	
	CDC *pDC = GetDC();
	OnPrepareDC(pDC);
	pDC->DPtoLP(&point);

	//state1: 处于捕获状态，画图
	if(GetCapture() == this){
		m_stroke->ReDrawStroke(pDC, point);
	}
	//state2: 非捕获状态，高亮
	else{	
		bool refresh = false;
		for(int i = 0; i < GetDocument()->m_strokeList.GetSize(); i ++)
		{
			if(GetDocument()->m_strokeList.GetAt(i)->IsPointIn(point)){
				//修改鼠标
				//SetCursor(LoadCursor(NULL, IDC_ARROW));
				//if state changed from NOT selected to selected
				if(!GetDocument()->m_strokeList.GetAt(i)->IsHightLight())
					refresh = true;
				GetDocument()->m_strokeList.GetAt(i)->SetHighLight(TRUE);
			}
			else{
				//if state changed from selected to NOT selected
				if(GetDocument()->m_strokeList.GetAt(i)->IsHightLight())
					refresh = true;
				GetDocument()->m_strokeList.GetAt(i)->SetHighLight(FALSE);
			}
		}
		if(refresh)
			Invalidate();
	}
	ReleaseDC(pDC);
	CView::OnMouseMove(nFlags, point);
}

void CHDrawView::SetDocument(CHDrawDoc *pDoc)
{
	pOldDoc = m_pDocument;
	m_pDocument = pDoc;
}


void CHDrawView::OnBmpSave() 
{
	// TODO: Add your command handler code here
	CFileDialog dlg(FALSE, "bmp","hjz.bmp");
	if(dlg.DoModal() != IDOK){
		return ;
	}
	CString filePath = dlg.GetPathName();
	// TODO: Add your command handler code here
	CClientDC client(this);//用于本控件的，楼主可以不用此句
	CDC cdc;
	CBitmap bitmap;
	RECT rect;CRect r;
	GetClientRect(&rect);
	int cx = rect.right - rect.left;
	int cy = rect.bottom - rect.top;
	bitmap.CreateCompatibleBitmap(&client, cx, cy);
	cdc.CreateCompatibleDC(NULL);
	CBitmap * oldbitmap = (CBitmap* ) cdc.SelectObject(&bitmap);
	cdc.FillRect(&rect, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	for(int i = 0; i < GetDocument()->m_strokeList.GetSize(); i ++){
		GetDocument()->m_strokeList.GetAt(i)->DrawStroke(&cdc);
	}
	cdc.SelectObject(oldbitmap);
	::OpenClipboard(this->m_hWnd);
	::EmptyClipboard();
	::SetClipboardData(CF_BITMAP, bitmap);
	::CloseClipboard();
	
	
	HBITMAP hBitmap = (HBITMAP)bitmap;
	HDC hDC;
	int iBits;
	WORD wBitCount;
	DWORD dwPaletteSize=0, dwBmBitsSize=0, dwDIBSize=0, dwWritten=0;
	BITMAP Bitmap; 
	BITMAPFILEHEADER bmfHdr; 
	BITMAPINFOHEADER bi; 
	LPBITMAPINFOHEADER lpbi; 
	HANDLE fh, hDib, hPal,hOldPal=NULL;
	
	hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)  wBitCount = 1;
	else if (iBits <= 4)  wBitCount = 4;
	else if (iBits <= 8)  wBitCount = 8;
	else      wBitCount = 24;
	
	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize   = sizeof(BITMAPINFOHEADER);
	bi.biWidth   = Bitmap.bmWidth;
	bi.biHeight   = Bitmap.bmHeight;
	bi.biPlanes   = 1;
	bi.biBitCount  = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage  = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed  = 0;
	
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
	
	hDib = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	
	GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
	
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}
	
	fh = CreateFile(filePath, GENERIC_WRITE,0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	
	if (fh == INVALID_HANDLE_VALUE)  
		return 		;
	
	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize; 
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh); 
}


void CHDrawView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	//context menu
	CMenu rmenu;
	rmenu.LoadMenu(IDR_MENU_SET);
	ClientToScreen(&point);
	rmenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	rmenu.Detach();
	CView::OnRButtonDown(nFlags, point);
}

void CHDrawView::OnPicColorBlue() 
{
	CHDrawDoc *pDoc = GetDocument();
	for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++)
		if(pDoc->m_strokeList.GetAt(i)->m_bSelected ||
			pDoc->m_strokeList.GetAt(i)->m_bHighLight)
			pDoc->m_strokeList.GetAt(i)->m_penColor = RGB(0,0,255);
	GetDocument()->BackUp();//备份
	Invalidate();
}

void CHDrawView::OnPicColorRed() 
{
	// TODO: Add your command handler code here
	CHDrawDoc *pDoc = GetDocument();
	for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++)
		if(pDoc->m_strokeList.GetAt(i)->m_bSelected ||
			pDoc->m_strokeList.GetAt(i)->m_bHighLight)
			pDoc->m_strokeList.GetAt(i)->m_penColor = RGB(255,0,0);
	GetDocument()->BackUp();//备份
	Invalidate();
}

void CHDrawView::OnPicColorGreen() 
{
	// TODO: Add your command handler code here
	CHDrawDoc *pDoc = GetDocument();
	for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++)
		if(pDoc->m_strokeList.GetAt(i)->m_bSelected ||
			pDoc->m_strokeList.GetAt(i)->m_bHighLight)
			pDoc->m_strokeList.GetAt(i)->m_penColor = RGB(0,255,0);
	GetDocument()->BackUp();//备份
	Invalidate();
}

void CHDrawView::OnPicColorOther() 
{
	// TODO: Add your command handler code here
	CColorDialog dlg;
	if(dlg.DoModal() == IDOK){
		CHDrawDoc *pDoc = GetDocument();
		for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++){
			if(pDoc->m_strokeList.GetAt(i)->m_bSelected ||
				pDoc->m_strokeList.GetAt(i)->m_bHighLight){
				pDoc->m_strokeList.GetAt(i)->m_penColor = dlg.GetColor();
			}
		}
		GetDocument()->BackUp();//备份
		Invalidate();
	}
}

void CHDrawView::OnPicSolid() 
{
	// TODO: Add your command handler code here
	CHDrawDoc *pDoc = GetDocument();
	for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++)
		if(pDoc->m_strokeList.GetAt(i)->m_bSelected ||
			pDoc->m_strokeList.GetAt(i)->m_bHighLight)
			pDoc->m_strokeList.GetAt(i)->m_penStyle = PS_SOLID;
	Invalidate();
}
void CHDrawView::OnPicDash() 
{
	// TODO: Add your command handler code here
	CHDrawDoc *pDoc = GetDocument();
	for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++)
		if(pDoc->m_strokeList.GetAt(i)->m_bSelected ||
			pDoc->m_strokeList.GetAt(i)->m_bHighLight)
			pDoc->m_strokeList.GetAt(i)->m_penStyle = PS_DASH;
	GetDocument()->BackUp();//备份
	Invalidate();
}



void CHDrawView::OnPicW1() 
{
	// TODO: Add your command handler code here
	CHDrawDoc *pDoc = GetDocument();
	for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++){
		if(pDoc->m_strokeList.GetAt(i)->m_bSelected ||
			pDoc->m_strokeList.GetAt(i)->m_bHighLight)
			pDoc->m_strokeList.GetAt(i)->m_penWidth = 1;
	}
	GetDocument()->BackUp();//备份
	Invalidate();
}

void CHDrawView::OnPicW2() 
{
	// TODO: Add your command handler code here
	CHDrawDoc *pDoc = GetDocument();
	for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++){
		if(pDoc->m_strokeList.GetAt(i)->m_bSelected ||
			pDoc->m_strokeList.GetAt(i)->m_bHighLight)
			pDoc->m_strokeList.GetAt(i)->m_penWidth = 2;
	}
	GetDocument()->BackUp();//备份
	Invalidate();
}

void CHDrawView::OnPicW3() 
{
	// TODO: Add your command handler code here
	CHDrawDoc *pDoc = GetDocument();
	for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++){
		if(pDoc->m_strokeList.GetAt(i)->m_bSelected ||
			pDoc->m_strokeList.GetAt(i)->m_bHighLight)
			pDoc->m_strokeList.GetAt(i)->m_penWidth = 3;
	}
	GetDocument()->BackUp();//备份
	Invalidate();
}


void CHDrawView::OnPicDelete() 
{
	// TODO: Add your command handler code here
	CHDrawDoc *pDoc = GetDocument();
	//remove all strokes
	bool refresh = false;
	for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++){
		if(pDoc->m_strokeList.GetAt(i)->m_bSelected ||
			pDoc->m_strokeList.GetAt(i)->m_bHighLight){
			delete pDoc->m_strokeList.GetAt(i);
			pDoc->m_strokeList.RemoveAt(i);
			i--;
			refresh = true;
		}
	}
	//refresh if needed
	if(refresh){
		GetDocument()->BackUp();//备份
		Invalidate();
	}
}



BOOL CHDrawView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(pWnd == this){
		CPoint point;
		//Step1. get cursor position
		GetCursorPos(&point);
		//Step2. convert point from screen to client
		ScreenToClient(&point);
		
		CDC* pDC = GetDC();//获得DC
		OnPrepareDC(pDC);//设置DC的滚动属性，与ScollView的滚动有关
		pDC->DPtoLP(&point);//转换当前点为逻辑位置坐标
		CHDrawDoc *pDoc = GetDocument();
		//Track判断
		for(int i = 0; i < pDoc->m_strokeList.GetSize(); i ++){
			if(pDoc->m_strokeList.GetAt(i)->m_bSelected && 
				pDoc->m_strokeList.GetAt(i)->m_tracker.HitTest(point) >= 0){
				pDoc->m_strokeList.GetAt(i)->m_tracker.SetCursor(pWnd, nHitTest);
				ReleaseDC(pDC);
				return TRUE;
			}
		}		
		ReleaseDC(pDC);
	}
	
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CHDrawView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	CHDrawDoc *pDoc = GetDocument();
	BOOL deleted = FALSE;
	int i, x, y;
	if   (pMsg->message   ==   WM_KEYDOWN)  
	{  
		switch (pMsg->wParam){
		case VK_DELETE:
			for(i = 0; i <pDoc->m_strokeList.GetSize(); i ++){
				if(pDoc->m_strokeList.GetAt(i)->m_bSelected){
					pDoc->m_strokeList.RemoveAt(i--);
					deleted = TRUE;
				}
			}
			if(deleted)
				Invalidate();
			break;
		case 'A':
		case 'a':
			if(::GetKeyState(VK_CONTROL) < 0){	
				for(int i = 0; i <pDoc->m_strokeList.GetSize(); i ++){
					pDoc->m_strokeList.GetAt(i)->m_bSelected = TRUE;
				}
				Invalidate();
			}
			break;
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			x = (pMsg->wParam==VK_RIGHT) - (pMsg->wParam==VK_LEFT);
			y = (pMsg->wParam==VK_DOWN) - (pMsg->wParam==VK_UP);
			if(::GetKeyState(VK_SHIFT) < 0){
				x *= 8;
				y *= 8;
			}
			for(int i = 0; i <pDoc->m_strokeList.GetSize(); i ++){
				if(pDoc->m_strokeList.GetAt(i)->m_bSelected){
					pDoc->m_strokeList.GetAt(i)->Move(x,y);
				}
			}
			Invalidate();
			break;
		}
	}
	return CView::PreTranslateMessage(pMsg);
}
