
// GenMatrixFontDlg.cpp : ��@��
//

#include "stdafx.h"
#include "GenMatrixFont.h"
#include "GenMatrixFontDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �� App About �ϥ� CAboutDlg ��ܤ��

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ܤ�����
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

// �{���X��@
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGenMatrixFontDlg ��ܤ��

int g_OldWidth = 0;
int g_OldHeigh = 0;

CGenMatrixFontDlg::CGenMatrixFontDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGenMatrixFontDlg::IDD, pParent)
	, m_TextWord(_T(""))
	, m_Width(16)
	, m_Heigh(16)
	, m_EditHex(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	g_OldWidth = m_Width;
	g_OldHeigh = m_Heigh;
}

void CGenMatrixFontDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_TextWord);
	DDV_MaxChars(pDX, m_TextWord, 1);
	DDX_Text(pDX, IDC_EDIT2, m_Width);
	DDV_MinMaxInt(pDX, m_Width, 8, 64);
	DDX_Text(pDX, IDC_EDIT3, m_Heigh);
	DDV_MinMaxInt(pDX, m_Heigh, 8, 64);
	DDX_Control(pDX, IDC_LIST1, m_ListMatrix);
	DDX_Text(pDX, IDC_EDIT4, m_EditHex);
}

BEGIN_MESSAGE_MAP(CGenMatrixFontDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CGenMatrixFontDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &CGenMatrixFontDlg::OnBnClickedCancel)
	ON_WM_MEASUREITEM()
	ON_BN_CLICKED(IDC_BUTTON2, &CGenMatrixFontDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CGenMatrixFontDlg �T���B�z�`��

BOOL CGenMatrixFontDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �N [����...] �\���[�J�t�Υ\���C

	// IDM_ABOUTBOX �����b�t�ΩR�O�d�򤧤��C
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �]�w����ܤ�����ϥܡC�����ε{�����D�������O��ܤ���ɡA
	// �ج[�|�۰ʱq�Ʀ��@�~
	SetIcon(m_hIcon, TRUE);			// �]�w�j�ϥ�
	SetIcon(m_hIcon, FALSE);		// �]�w�p�ϥ�

	// TODO: �b���[�J�B�~����l�]�w
	InitFont();

	InitListCols();

	return TRUE;  // �Ǧ^ TRUE�A���D�z�ﱱ��]�w�J�I
}

void CGenMatrixFontDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	//
	// OnMeasureItem will be called only if the control's class is created at run time, 
	// or it is created with the LBS_OWNERDRAWVARIABLE or CBS_OWNERDRAWVARIABLE style. 
	// If the control is created by the dialog editor, OnMeasureItem will not be called. 
	// This is because the WM_MEASUREITEM message is sent early in the creation process of the control.

	if( nIDCtl == IDC_LIST1 )
	{
		lpMeasureItemStruct->itemHeight = 12;
	}

	CDialogEx::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CGenMatrixFontDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �p�G�N�̤p�ƫ��s�[�J�z����ܤ���A�z�ݭn�U�C���{���X�A
// �H�Kø�s�ϥܡC���ϥΤ��/�˵��Ҧ��� MFC ���ε{���A
// �ج[�|�۰ʧ������@�~�C

void CGenMatrixFontDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ø�s���˸m���e

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N�ϥܸm����Τ�ݯx��
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �yø�ϥ�
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ��ϥΪ̩즲�̤p�Ƶ����ɡA
// �t�ΩI�s�o�ӥ\����o�����ܡC
HCURSOR CGenMatrixFontDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGenMatrixFontDlg::InitFont()
{
	HDC hDC = GetDC()->GetSafeHdc();
	m_hdcMemory = CreateCompatibleDC( hDC );
	SetMapMode( m_hdcMemory, MM_TEXT );
	m_bitmap = CreateCompatibleBitmap(hDC, m_Width, m_Heigh);
	SelectObject(m_hdcMemory, m_bitmap);
	Rectangle( m_hdcMemory, 0 , 0, m_Width, m_Heigh );

	//Arial, �з��� ,CHINESEBIG5_CHARSET, 500
	if(m_Heigh >= 16)
		m_hFont = CreateFont(m_Heigh, 0, 0, 0, 0, false, false, false, CHINESEBIG5_CHARSET, 0, 0, 0, 0, L"Small SimSun" );
	else
		m_hFont = CreateFont(m_Heigh, 0, 0, 0, 0, false, false, false, ANSI_CHARSET, 0, 0, 0, 0, L"Small SimSun" );

	SelectObject( m_hdcMemory, m_hFont );
	SetTextAlign( m_hdcMemory, TA_LEFT | TA_TOP );
	SetTextColor( m_hdcMemory, RGB( 0, 0, 0 ) );
}

void CGenMatrixFontDlg::DrawFont()
{
	TextOut( m_hdcMemory, 0, 0, L" ", 3);
	if(m_TextWord == "")
		m_TextWord = " ";
	TextOut( m_hdcMemory, 0, 0, m_TextWord, 1 );

	InsertListItems(m_hdcMemory);
}

void CGenMatrixFontDlg::InitListCols()
{
	// Create 256 color image lists
	HIMAGELIST hList = ImageList_Create(12, 12, ILC_COLOR32 | ILC_MASK, 2, 1);
	m_cImageListSmall.Attach(hList);

	// Load the large icons
	CBitmap cBmp1, cBmp2;
	cBmp1.LoadBitmap(IDB_BMP_GRAY);
	cBmp2.LoadBitmap(IDB_BMP_RED);
	m_cImageListSmall.Add(&cBmp1, RGB(255,255,255));
	m_cImageListSmall.Add(&cBmp2, RGB(255,255,255));
	cBmp1.DeleteObject();
	cBmp2.DeleteObject();

	//m_ListMatrix.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | LVS_OWNERDRAWFIXED | WS_BORDER | WS_TABSTOP,
	//					CRect(8,35,352,295), this, IDC_LIST1);  

	m_ListMatrix.SetImageList(&m_cImageListSmall, LVSIL_NORMAL);
	m_ListMatrix.SetImageList(&m_cImageListSmall, LVSIL_SMALL);
	//Set the extended styles
	m_ListMatrix.ModifyStyle( LVS_OWNERDRAWFIXED, 0, 0 );
    m_ListMatrix.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES ); //LVS_EX_FULLROWSELECT

	InsertColumns();
}

void CGenMatrixFontDlg::InsertColumns()
{
	// Insert some columns
	m_ListMatrix.InsertColumn(0, _T(""), LVCFMT_IMAGE|LVCFMT_BITMAP_ON_RIGHT, 16);
	for(int i = 1; i < m_Width; ++ i)
		m_ListMatrix.InsertColumn(i, _T(""), LVCFMT_IMAGE|LVCFMT_BITMAP_ON_RIGHT, 12);

	m_ListMatrix.InsertColumn(m_Width, _T(""), LVCFMT_RIGHT, 38);
	m_ListMatrix.InsertColumn(m_Width + 1, _T(""), LVCFMT_RIGHT, 38);
}

void CGenMatrixFontDlg::InsertListItems(const HDC& hdc)
{
	// Delete the current contents
	if(m_ListMatrix.GetItemCount())
		m_ListMatrix.DeleteAllItems();

	//m_EditHex = "";
	char dot=',';
	unsigned char hex;
	LVITEM lvi;
	lvi.mask =  LVIF_IMAGE;//|LVIF_TEXT;
	LVITEM lviText;
	lviText.mask = LVIF_TEXT;
	m_EditHex.AppendFormat(L"{");
	CString HexStr = L"";
	for (int i = 0; i < m_Width; i++)
	{
		// Insert the first item
		lvi.iItem = i;
		hex = 0;
		for (int j = 0; j < m_Heigh; j++)
		{
			lvi.iSubItem = j;
			lvi.iImage = ((GetPixel( hdc, j, i )&0xff) == 0xff) ? 0 : 1;
			if(j==0)
				m_ListMatrix.InsertItem(&lvi);
			else
				m_ListMatrix.SetItem(&lvi);

			hex <<= 1;
			if(lvi.iImage)
				hex += 1;

			if(j == (m_Heigh/2 - 1) || j == (m_Heigh - 1))
			{
				if(i == (m_Width - 1) &&  j == (m_Heigh - 1) )
					dot = ' ';
				else
					dot = ',';
				m_EditHex.AppendFormat(L"0x%02x%c", hex, dot);
				

				HexStr.Format(L"0x%02x", hex);
				lviText.iItem = i;
				lviText.pszText = (LPWSTR)HexStr.GetBuffer(0);
				if(j == (m_Heigh/2 - 1))
				{
					lviText.iSubItem = m_Width;
					m_ListMatrix.SetItem(&lviText);
				}
				else if(j == (m_Heigh - 1))
				{
					lviText.iSubItem = m_Width + 1;
					m_ListMatrix.SetItem(&lviText);
				}

				hex = 0;
			}
		}
		if(i == (m_Width/2 - 1))
			m_EditHex.AppendFormat(L"\r\n");
	}
	m_EditHex.AppendFormat(L"}");
	m_EditHex.AppendFormat(L"\r\n");
	UpdateData(FALSE);
}

void CGenMatrixFontDlg::OnBnClickedButton1()
{
	// TODO: �b���[�J����i���B�z�`���{���X
	UpdateData();
	if( g_OldWidth != m_Width || g_OldHeigh != m_Heigh )
	{
		CHeaderCtrl* m_Header=(CHeaderCtrl*)m_ListMatrix.GetDlgItem(0);
		int nCount = m_Header->GetItemCount();
		for(int i = nCount - 1; i >= 0; --i)
			m_ListMatrix.DeleteColumn(i);

		InsertColumns();

		g_OldWidth = m_Width;
		g_OldHeigh = m_Heigh;
		ReleaseObj();
		InitFont();
	}

	UpdateData();
	DrawFont();
}

void CGenMatrixFontDlg::OnBnClickedCancel()
{
	// TODO: �b���[�J����i���B�z�`���{���X
	CDialogEx::OnCancel();
	ReleaseObj();
}

void CGenMatrixFontDlg::ReleaseObj()
{
	DeleteObject(m_hFont);
	DeleteObject(m_bitmap);
	DeleteObject(m_hdcMemory);
	m_hFont = 0;
	m_bitmap = 0;
	m_hdcMemory = 0;
}

void CGenMatrixFontDlg::OnBnClickedButton2()
{
	// TODO: �b���[�J����i���B�z�`���{���X
	m_EditHex = L"";
	UpdateData(FALSE);
}
