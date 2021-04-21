// HStrokeTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HDraw.h"
#include "HStrokeTextDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HStrokeTextDlg dialog


HStrokeTextDlg::HStrokeTextDlg(CWnd* pParent /*=NULL*/)
	: CDialog(HStrokeTextDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(HStrokeTextDlg)
	m_editText = _T("");
	//}}AFX_DATA_INIT
}


void HStrokeTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(HStrokeTextDlg)
	DDX_Text(pDX, IDC_EDIT_TEXT, m_editText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(HStrokeTextDlg, CDialog)
	//{{AFX_MSG_MAP(HStrokeTextDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HStrokeTextDlg message handlers



