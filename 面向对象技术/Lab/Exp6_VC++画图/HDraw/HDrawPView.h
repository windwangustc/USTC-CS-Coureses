#if !defined(AFX_HDRAWPVIEW_H__FEB65AFC_F382_46FD_9DCD_67CD1D24062C__INCLUDED_)
#define AFX_HDRAWPVIEW_H__FEB65AFC_F382_46FD_9DCD_67CD1D24062C__INCLUDED_

#include "HDrawView.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HDrawPView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHDrawPView view

class CHDrawPView : public CScrollView
{
protected:
	CHDrawPView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CHDrawPView)

// Attributes
public:

// Operations
public:
	CHDrawDoc* GetDocument();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHDrawPView)
	public:
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CHDrawView *m_drawView;
	CRectTracker m_tracker;
	virtual ~CHDrawPView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CHDrawPView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HDRAWPVIEW_H__FEB65AFC_F382_46FD_9DCD_67CD1D24062C__INCLUDED_)
