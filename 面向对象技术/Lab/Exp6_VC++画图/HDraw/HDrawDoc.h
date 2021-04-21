// HDrawDoc.h : interface of the CHDrawDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HDRAWDOC_H__70CA8453_799B_44C5_BE7E_3965B9DE3A55__INCLUDED_)
#define AFX_HDRAWDOC_H__70CA8453_799B_44C5_BE7E_3965B9DE3A55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HStroke.h"
#include "HStrokeEditDlg.h"
#include "HStrokeTextDlg.h"

class CHDrawDoc : public CDocument
{
protected: // create from serialization only
	CHDrawDoc();
	DECLARE_DYNCREATE(CHDrawDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHDrawDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_backup_index;
	void ReStore(BOOL backward);
	void BackUp();
	CStringArray m_backup;
	int m_picStyle;
	UINT m_cavasH;
	UINT m_cavasW;
	UINT m_picWidth;
	COLORREF m_picColor;
	CString m_picInfo;
	HStroke * NewStroke();
	HPicType m_picType;
	CTypedPtrArray<CObArray, HStroke *> m_strokeList;
	virtual ~CHDrawDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CHDrawDoc)
	afx_msg void OnDrawClear();
	afx_msg void OnDrawEllipse();
	afx_msg void OnDrawLine();
	afx_msg void OnDrawPoly();
	afx_msg void OnDrawRect();
	afx_msg void OnDrawSet();
	afx_msg void OnDrawText();
	afx_msg void OnBtW1();
	afx_msg void OnBtW2();
	afx_msg void OnBtW3();
	afx_msg void OnUpdateBtW1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtW2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtW3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawEllipse(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawPoly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawRect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawText(CCmdUI* pCmdUI);
	afx_msg void OnPenDash();
	afx_msg void OnUpdatePenDash(CCmdUI* pCmdUI);
	afx_msg void OnPenSolid();
	afx_msg void OnUpdatePenSolid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnEditUndo();
	afx_msg void OnSelect();
	afx_msg void OnUpdateSelect(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HDRAWDOC_H__70CA8453_799B_44C5_BE7E_3965B9DE3A55__INCLUDED_)
