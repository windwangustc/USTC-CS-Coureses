#if !defined(AFX_HStrokeEditDlg_H__FAFB5955_AE6B_482E_BBE2_8DCE58BB257A__INCLUDED_)
#define AFX_HStrokeEditDlg_H__FAFB5955_AE6B_482E_BBE2_8DCE58BB257A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HStrokeEditDlg.h : header file
//
#include "HStroke.h"
#include "math.h"

/////////////////////////////////////////////////////////////////////////////
// HStrokeEditDlg dialog

class HStrokeEditDlg : public CDialog
{
// Construction
public:
	void setStrokeList(CTypedPtrArray<CObArray, HStroke *> *p);
	HStrokeEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(HStrokeEditDlg)
	enum { IDD = IDD_DLG_SET };
	CButton	m_btnColor;
	CEdit	m_editWidth;
	CListBox	m_listStroke;
	CEdit	m_editStroke;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HStrokeEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
private:
	CString strokeInfo(HStroke *stroke, int index);
// Implementation
protected:
	CTypedPtrArray<CObArray, HStroke *> * m_ptrStrokeList;
	// Generated message map functions
	//{{AFX_MSG(HStrokeEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListStrokes();
	afx_msg void OnBtnUpdate();
	afx_msg void OnBtnDel();
	afx_msg void OnBtnColor();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HStrokeEditDlg_H__FAFB5955_AE6B_482E_BBE2_8DCE58BB257A__INCLUDED_)
