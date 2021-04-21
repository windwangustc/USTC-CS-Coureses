// HDrawView.h : interface of the CHDrawView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HDRAWVIEW_H__E49A37E8_F413_47C5_A736_5AB3441146E2__INCLUDED_)
#define AFX_HDRAWVIEW_H__E49A37E8_F413_47C5_A736_5AB3441146E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MainFrm.h"
#include "HDrawDoc.h"
#include "HDrawView.h"

class CHDrawView : public CView
{
public: //change to public, because it is used by others
	CHDrawView();
// Attributes
public:

// Operations
public:
	CHDrawDoc* GetDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHDrawView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetDocument(CHDrawDoc *pDoc);
	CPen m_pen;
	HStroke * m_stroke;	
	//CTypedPtrArray<CObArray, HStroke *> m_strokeSelected;
	virtual ~CHDrawView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	CDocument * pOldDoc;
	//{{AFX_MSG(CHDrawView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBmpSave();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPicColorBlue();
	afx_msg void OnPicColorGreen();
	afx_msg void OnPicColorOther();
	afx_msg void OnPicColorRed();
	afx_msg void OnPicDash();
	afx_msg void OnPicSolid();
	afx_msg void OnPicW1();
	afx_msg void OnPicW2();
	afx_msg void OnPicW3();
	afx_msg void OnPicDelete();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in HDrawView.cpp
inline CHDrawDoc* CHDrawView::GetDocument()
   { return (CHDrawDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HDRAWVIEW_H__E49A37E8_F413_47C5_A736_5AB3441146E2__INCLUDED_)
