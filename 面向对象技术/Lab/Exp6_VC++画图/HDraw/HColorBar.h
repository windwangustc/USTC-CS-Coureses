#if !defined(AFX_HCOLORBAR_H__25DEB863_33D8_44CA_89E9_779A6012C9A0__INCLUDED_)
#define AFX_HCOLORBAR_H__25DEB863_33D8_44CA_89E9_779A6012C9A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HColorBar.h : header file
//
#include "MainFrm.h"

/////////////////////////////////////////////////////////////////////////////
// HColorBar window
#define COLORNUMBER 28
class HColorBar : public CToolBar
{
// Construction
public:
	HColorBar();
	virtual ~HColorBar();
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	void UpdateDocument();
	COLORREF GetColor();//get method

private:
	COLORREF m_COLOR[COLORNUMBER];	//colors of platte
	COLORREF m_currentColor;		//current color
    CRect m_rect[COLORNUMBER];		//rect  of platte
	CRect m_rectAll;				//total area of rect
	// Generated message map functions
protected:
	//{{AFX_MSG(HColorBar)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HCOLORBAR_H__25DEB863_33D8_44CA_89E9_779A6012C9A0__INCLUDED_)
