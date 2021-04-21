#if !defined(AFX_HSTROKETEXTDLG_H__31310CB0_61CA_410C_ACD4_6857D844404D__INCLUDED_)
#define AFX_HSTROKETEXTDLG_H__31310CB0_61CA_410C_ACD4_6857D844404D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HStrokeTextDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// HStrokeTextDlg dialog

class HStrokeTextDlg : public CDialog
{
// Construction
public:
	HStrokeTextDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(HStrokeTextDlg)
	enum { IDD = IDD_DLG_TEXT };
	CString	m_editText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HStrokeTextDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(HStrokeTextDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HSTROKETEXTDLG_H__31310CB0_61CA_410C_ACD4_6857D844404D__INCLUDED_)
