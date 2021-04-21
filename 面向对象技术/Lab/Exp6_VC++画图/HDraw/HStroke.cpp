// HStroke.cpp: implementation of the HStroke class.
// Download by http://www.codefans.net
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HDraw.h"
#include "HStroke.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(HStroke, CObject, 1 )
HStroke::HStroke()
{
	m_penColor = RGB(255, 0, 0);
	m_penWidth = 2;
	m_penStyle = PS_SOLID;
	m_bHighLight = FALSE;
	m_bSelected = FALSE;
}

HStroke::~HStroke()
{

}

void HStroke::DrawStroke(CDC *pDC)
{
	CPen *pOld, pNew; 
	pNew.CreatePen(m_penStyle, m_penWidth, m_penColor);
	pOld = pDC->SelectObject(&pNew);
	_Draw(pDC);
	if((m_bSelected||m_bHighLight) && m_points.GetSize() == 2){
		m_tracker.m_nStyle = CRectTracker::resizeInside;
		m_tracker.m_rect.SetRect(m_points.GetAt(0), m_points.GetAt(1));
		m_tracker.m_rect.NormalizeRect();
		m_tracker.Draw(pDC);
	}
	pDC->SelectObject(pOld);
}

void HStroke::ReDrawStroke(CDC *pDC, CPoint point)
{
	
}
void HStroke::_Draw(CDC *pDC)
{
}

void HStroke::SetCurrentPoint(CPoint point)
{
	m_points.Add(point);
}

void HStroke::Serialize(CArchive& ar)
{
	if(ar.IsStoring()){
		int enumIndex = m_picType;
		ar<<enumIndex<<m_penWidth<<m_penColor;
		m_points.Serialize(ar);
	}
	else{
		int enumIndex;
		ar>>enumIndex>>m_penWidth>>m_penColor;
		m_picType = (enum HPicType)enumIndex;
		m_points.Serialize(ar);
	}
}
//////////////////////////////////////////////////////////////////////
// HStrokeLine Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(HStrokeLine, CObject, 1)
HStrokeLine::HStrokeLine()
{
	m_picType = PIC_line;
	m_tracker.m_picType = m_picType;
}

HStrokeLine::~HStrokeLine()
{

}
void HStrokeLine::SetCurrentPoint(CPoint point){
	//空
	if(m_points.GetSize() < 2){
		m_points.Add(point);
	}
	else{
		m_points.ElementAt(1) = point;
	}
}
void HStrokeLine::DrawStroke(CDC *pDC){
	//左上->右下(0) or 左下-右上(1)
	// (x1-x2)*(y1-y2) > 0
	m_tracker.m_picExtra = (m_points.GetAt(0).x - m_points.GetAt(1).x) * (m_points.GetAt(0).y - m_points.GetAt(1).y) < 0;
	HStroke::DrawStroke(pDC);
}
void HStrokeLine::_Draw(CDC *pDC){
	pDC->MoveTo(m_points.GetAt(0));
	pDC->LineTo(m_points.GetAt(1));
}
void HStrokeLine::ReDrawStroke(CDC *pDC, CPoint point){
	if(m_points.GetSize() < 2){
		if(m_points.GetSize() == 1){
			m_points.Add(point);
			DrawStroke(pDC);
		}
		return ;
	}
 	pDC->SetROP2(R2_NOTXORPEN);

	DrawStroke(pDC);
	SetCurrentPoint(point);
	DrawStroke(pDC);
	
}

BOOL HStrokeLine::IsPointIn(const CPoint &point){
	int x1 = point.x - m_points.GetAt(0).x;
	int x2 = point.x - m_points.GetAt(1).x;
	int y1 = point.y - m_points.GetAt(0).y;
	int y2 = point.y - m_points.GetAt(1).y;
	//y1/x1=y2/x2 => x1*y2=x2*y1
	int measure = x1*y2 - x2*y1;

	//tolerable distance
	int rule = abs(m_points.GetAt(1).x - m_points.GetAt(0).x)
		+abs(m_points.GetAt(0).y - m_points.GetAt(1).y);
	rule *= m_penWidth;

	if(measure < rule && measure > -rule){
		//between the two points
		if(x1 * x2 < 0)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}
//////////////////////////////////////////////////////////////////////
// HStrokePoly Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(HStrokePoly, HStroke, 1)
HStrokePoly::HStrokePoly()
{
	m_picType = PIC_poly;
}

HStrokePoly::~HStrokePoly()
{

}
void HStrokePoly::SetCurrentPoint(CPoint point){
	m_points.Add(point);
}
void HStrokePoly::_Draw(CDC *pDC){
	if(m_points.GetSize() < 2)
		return ;
	pDC->MoveTo(m_points.GetAt(0));
	for(int i = 1; i < m_points.GetSize(); i ++)
		pDC->LineTo(m_points.GetAt(i));
}

void HStrokePoly::ReDrawStroke(CDC *pDC, CPoint point){
	if(m_points.GetSize() < 2)
		SetCurrentPoint(point);
	DrawStroke(pDC);
	SetCurrentPoint(point);
}
BOOL HStrokePoly::IsPointIn(const CPoint &point){
	for(int i = 0; i < m_points.GetSize(); i ++){
		if(m_points.GetAt(i) == point)
			return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
// HStrokeRect Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(HStrokeRect, HStrokeLine, 1)
HStrokeRect::HStrokeRect()
{
	m_picType = PIC_rect;
	m_tracker.m_picType = m_picType;
}

HStrokeRect::~HStrokeRect()
{

}
void HStrokeRect::_Draw(CDC *pDC){
	if(m_points.GetSize() < 2)
		return ;
	pDC->SelectStockObject(NULL_BRUSH);
	pDC->Rectangle(m_points.GetAt(0).x,
		m_points.GetAt(0).y,
		m_points.GetAt(1).x,
		m_points.GetAt(1).y);
}
void HStrokeRect::ReDrawStroke(CDC *pDC, CPoint point){
	if(m_points.GetSize() < 2){
		if(m_points.GetSize() == 1){
			SetCurrentPoint(point);
			DrawStroke(pDC);
		}
		return ;
	}
	pDC->SetROP2(R2_NOTXORPEN);
	DrawStroke(pDC);
	SetCurrentPoint(point);
	DrawStroke(pDC);
}
BOOL HStrokeRect::IsPointIn(const CPoint &point){
	CRect rect(m_points.GetAt(0), m_points.GetAt(1));
	CRect rect2(rect.left + 5, rect.top+5, rect.right-5, rect.bottom-5);
	if(rect.PtInRect(point) && !rect2.PtInRect(point))
		return TRUE;
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////
// HStrokeEllipse Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(HStrokeEllipse, CObject, 1)
HStrokeEllipse::HStrokeEllipse()
{
	m_picType = PIC_ellipse;
	m_tracker.m_picType = m_picType;
}

HStrokeEllipse::~HStrokeEllipse()
{

}
void HStrokeEllipse::_Draw(CDC *pDC){
	if(m_points.GetSize() < 2)
		return ;
	pDC->SelectStockObject(NULL_BRUSH);
	pDC->Ellipse(m_points.GetAt(0).x,
		m_points.GetAt(0).y,
		m_points.GetAt(1).x,
		m_points.GetAt(1).y);
}
void HStrokeEllipse::ReDrawStroke(CDC *pDC, CPoint point){
	if(m_points.GetSize() < 2){
		if(m_points.GetSize() == 1){
			SetCurrentPoint(point);
			DrawStroke(pDC);
		}
		return ;
	}
	pDC->SetROP2(R2_NOTXORPEN);
	DrawStroke(pDC);
	SetCurrentPoint(point);
	DrawStroke(pDC);
}
BOOL HStrokeEllipse::IsPointIn(const CPoint &point){
	//calculate a,b,c of Ellipse
	int _2a = abs(m_points.GetAt(0).x - m_points.GetAt(1).x);
	int _2b = abs(m_points.GetAt(0).y - m_points.GetAt(1).y);
	double c = sqrt((double)abs(_2a*_2a - _2b*_2b))/2;
	//calculate two points
	double x1,y1,x2,y2;
	if(_2a > _2b){
		x1 = (double)(m_points.GetAt(0).x + m_points.GetAt(1).x)/2 - c;
		x2 = x1 + 2*c;
		y1 = y2 = (m_points.GetAt(0).y + m_points.GetAt(1).y)/2;
	}
	else{
		_2a = _2b;
		x1 = x2 = (m_points.GetAt(0).x + m_points.GetAt(1).x)/2;
		y1 = (m_points.GetAt(0).y + m_points.GetAt(1).y)/2 - c;
		y2 = y1 + 2*c;
	}
	//distance(point - p1) + distance(point - p2) = 2*a;
	double measure = sqrt((x1 - point.x)*(x1-point.x) + (y1 - point.y)*(y1-point.y) )
		+ sqrt( (point.x - x2)*(point.x - x2) + (point.y - y2)*(point.y - y2)) 
		- _2a;
	double rule = 4*m_penWidth;
	if(measure <  rule && measure > -rule)
		return TRUE;
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////
// HStrokeText Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(HStrokeText, HStrokeLine, 1)
HStrokeText::HStrokeText()
{
	m_picType = PIC_text;
}
HStrokeText::HStrokeText(CString info)
{
	m_text = info;
	m_picType = PIC_text;
}
HStrokeText::~HStrokeText()
{

}
void HStrokeText::DrawStroke(CDC *pDC){
	if(m_points.GetSize() < 2)
		return ;
	CRect rect(m_points.GetAt(0).x,
		m_points.GetAt(0).y,
		m_points.GetAt(1).x,
		m_points.GetAt(1).y);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_penColor);
	CFont textFont, *pOldFont;
	textFont.CreatePointFont(m_penWidth*100, "Times New Roman");

	pOldFont = pDC->SelectObject(&textFont);
	pDC->DrawText(m_text, rect, DT_LEFT);
	pDC->SelectObject(pOldFont);
}
void HStrokeText::ReDrawStroke(CDC *pDC, CPoint point){
	if(m_points.GetSize() < 2){
		if(m_points.GetSize() == 1){
			SetCurrentPoint(point);
			DrawStroke(pDC);
		}
		return ;
	}
	pDC->SetROP2(R2_XORPEN);
	DrawStroke(pDC);
	SetCurrentPoint(point);
	DrawStroke(pDC);
}
void HStrokeText::Serialize(CArchive& ar)
{
	HStroke::Serialize(ar);
	if(ar.IsStoring())
		ar<<m_text;
	else
		ar>>m_text;
}

BOOL HStroke::IsPointIn(const CPoint &point)
{
	return false;
}


void HStroke::SetHighLight(BOOL bHL)
{
	m_bHighLight = bHL;
}

BOOL HStroke::IsHightLight()
{
	return m_bHighLight;
}

void HStroke::Move(int x, int y)
{
	for(int i = 0; i < m_points.GetSize(); i ++){
		m_points.ElementAt(i).x += x;
		m_points.ElementAt(i).y += y;
	}
}

void HStroke::ReSize(CRect newPos)
{
	ASSERT(m_points.GetSize() >= 2);
	//左下到右上
	if((m_points[0].x - m_points[1].x) * (m_points[0].y - m_points[1].y) < 0)
	{
		m_points.ElementAt(0) = CPoint(newPos.left, newPos.bottom);
		m_points.ElementAt(1) = CPoint(newPos.right, newPos.top);
	}
	else
	{
		m_points.ElementAt(0) = CPoint(newPos.left, newPos.top);
		m_points.ElementAt(1) = CPoint(newPos.right, newPos.bottom);
	}
}

HStrokeSelect::HStrokeSelect()
{
	m_picType = PIC_select;
}
void HStrokeSelect::DrawStroke(CDC *pDC)
{
	m_penColor = RGB(255,0,0);
	m_penWidth = 1;
	m_penStyle = PS_DASH;
	HStrokeRect::DrawStroke(pDC);
}
HStrokeTracker::HStrokeTracker():CRectTracker(){
	m_picType = PIC_rect;
}
void HStrokeTracker::Draw(CDC* pDC) const
{	
	CRect rect;
	CRectTracker::Draw(pDC);

	//直线
	if((m_picType == PIC_line) && ((m_nStyle&(resizeInside|resizeOutside))!=0))
	{
		UINT mask = GetHandleMask();
		for (int i = 0; i < 8; ++i)
		{
			if (mask & (1<<i))
			{
				int p1, p2;
				//左上+右下
				if(m_picExtra == 0)
				{
					p1 = 1, p2 = 4;
				}
				//左下+右上
				else
				{
					p1 = 2, p2 = 8;
				}
				if( ((1<<i) == p1) || ((1<<i) == p2))
				{
					GetHandleRect((TrackerHit)i, &rect);
					pDC->FillSolidRect(rect, RGB(0, 0, 0));
				}
				else
				{
					GetHandleRect((TrackerHit)i, &rect);
					pDC->FillSolidRect(rect, RGB(255, 255, 255));
				}
			}
		}
	}
}
