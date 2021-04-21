// HDraw.h : main header file for the HDRAW application
//

#if !defined(AFX_HDRAW_H__58B04A30_10D5_437A_B1FA_7873A78CA167__INCLUDED_)
#define AFX_HDRAW_H__58B04A30_10D5_437A_B1FA_7873A78CA167__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CHDrawApp:
// See HDraw.cpp for the implementation of this class
//

class CHDrawApp : public CWinApp
{
public:
	CHDrawApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHDrawApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CHDrawApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HDRAW_H__58B04A30_10D5_437A_B1FA_7873A78CA167__INCLUDED_)
