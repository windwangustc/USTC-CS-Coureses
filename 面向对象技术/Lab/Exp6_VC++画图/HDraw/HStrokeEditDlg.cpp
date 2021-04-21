// HStrokeEditDlg.cpp : implementation file
// Download by http://www.codefans.net
 
#include "stdafx.h"
#include "HDraw.h"
#include "HStrokeEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HStrokeEditDlg dialog


HStrokeEditDlg::HStrokeEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(HStrokeEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(HStrokeEditDlg)
	//}}AFX_DATA_INIT
}


void HStrokeEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(HStrokeEditDlg)
	DDX_Control(pDX, IDC_BTN_COLOR, m_btnColor);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_editWidth);
	DDX_Control(pDX, IDC_LIST_STROKES, m_listStroke);
	DDX_Control(pDX, IDC_EDIT_STROKE, m_editStroke);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(HStrokeEditDlg, CDialog)
	//{{AFX_MSG_MAP(HStrokeEditDlg)
	ON_LBN_SELCHANGE(IDC_LIST_STROKES, OnSelchangeListStrokes)
	ON_BN_CLICKED(IDC_BTN_UPDATE, OnBtnUpdate)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBtnDel)
	ON_BN_CLICKED(IDC_BTN_COLOR, OnBtnColor)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(ID_OK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HStrokeEditDlg message handlers

BOOL HStrokeEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	for(int i = 0; i < m_ptrStrokeList->GetSize(); i ++){
		m_listStroke.AddString(strokeInfo(m_ptrStrokeList->GetAt(i), i));
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void HStrokeEditDlg::setStrokeList(CTypedPtrArray<CObArray, HStroke *> *p)
{
	m_ptrStrokeList = p;
}

CString HStrokeEditDlg::strokeInfo(HStroke *stroke, int index)
{
	CString info, result;
	switch (stroke->m_picType)
	{
	case PIC_line:
		info = "直线";
		break;
	case PIC_poly:
		info = "曲线";
		break;
	case PIC_rect:
		info = "矩形";
		break;
	case PIC_ellipse:
		info = "椭圆";
		break;
	case PIC_text:
		info = "文本";
		break;
	default:
		info = "错误";
	}
	result.Format("%s     %d", info, index);
	return result;
}

void HStrokeEditDlg::OnSelchangeListStrokes() 
{
	// TODO: Add your control notification handler code here
	CString info, result;
	int i, index = m_listStroke.GetCurSel();
	HStroke *stroke = m_ptrStrokeList->ElementAt(index);
	//显示点集
	for(i = 0; i < stroke->m_points.GetSize(); i ++){
		info.Format(" [%d,%d]", 
			stroke->m_points.GetAt(i).x,
			stroke->m_points.GetAt(i).y);
		result += info;
	}
	m_editStroke.SetWindowText(result);
	//显示颜色
	info.Format("%d", stroke->m_penWidth);
	//显示线宽
	m_editWidth.SetWindowText(info);
	//选中某个图形
	for(i = 0; i <m_ptrStrokeList->GetSize(); i ++)
		m_ptrStrokeList->GetAt(i)->m_bSelected = FALSE;
	m_ptrStrokeList->GetAt(index)->m_bSelected = TRUE;
	GetParent()->Invalidate();
}

void HStrokeEditDlg::OnBtnUpdate() 
{
	// TODO: Add your control notification handler code here
	int pos, index = m_listStroke.GetCurSel();

	CString info, strPoint;
	//修改线宽
	m_editWidth.GetWindowText(info);
	CString m_width = info;
	if(index == LB_ERR)
		return ;
	m_ptrStrokeList->ElementAt(index)->m_penWidth = atoi(m_width);	

	m_editStroke.GetWindowText(info);
	CPoint point;
	//修改点集
	m_ptrStrokeList->ElementAt(index)->m_points.RemoveAll();
	while( (pos=info.Find("]")) != -1){
		strPoint = info.Left(pos);
		info = info.Right(info.GetLength() - strPoint.GetLength() - 1);

		point.x = atoi(strPoint.Mid(2, strPoint.Find(",")));
		point.y = atoi(strPoint.Mid(strPoint.Find(",")+1));
		m_ptrStrokeList->ElementAt(index)->m_points.Add(point);
	}
	GetParent()->Invalidate();
}

void HStrokeEditDlg::OnBtnDel() 
{
	// TODO: Add your control notification handler code here
	int index = m_listStroke.GetCurSel();
	//in condition of null selection
	if(LB_ERR == index)
		return ;
	//must delete this, otherwise there will be memory leak
	delete m_ptrStrokeList->ElementAt(index);
	//remove picture
	m_ptrStrokeList->RemoveAt(index);
	//refresh list
	m_listStroke.DeleteString(index);
	GetParentFrame()->Invalidate();
}

void HStrokeEditDlg::OnBtnColor() 
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg;
	int curIndex = m_listStroke.GetCurSel();
	if(LB_ERR != curIndex){
		dlg.m_cc.Flags |= CC_RGBINIT;
		dlg.m_cc.rgbResult = m_ptrStrokeList->GetAt(curIndex)->m_penColor;
	}
	if(dlg.DoModal() == IDOK){
		if(curIndex != LB_ERR){
			m_ptrStrokeList->ElementAt(curIndex)->m_penColor = dlg.GetColor();
			Invalidate();
		}
	}
}



HBRUSH HStrokeEditDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	int curIndex = m_listStroke.GetCurSel();
	if(curIndex != LB_ERR && pWnd->GetDlgCtrlID() == IDC_EDIT_WIDTH){
		pDC->SetTextColor(m_ptrStrokeList->ElementAt(curIndex)->m_penColor);
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void HStrokeEditDlg::OnOk() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}
