
// GenMatrixFontDlg.h : ���Y��
//

#pragma once
#include "afxcmn.h"


// CGenMatrixFontDlg ��ܤ��
class CGenMatrixFontDlg : public CDialogEx
{
// �غc
public:
	CGenMatrixFontDlg(CWnd* pParent = NULL);	// �зǫغc�禡

// ��ܤ�����
	enum { IDD = IDD_GENMATRIXFONT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �䴩

private:
	void InitFont();
	void DrawFont();
	void ReleaseObj();
	void InitListCols();
	void InsertListItems(const HDC& hdc);
	void InsertColumns();

// �{���X��@
protected:
	HICON m_hIcon;

	// ���ͪ��T�������禡
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton2();

private:
	HFONT m_hFont;
	HDC	m_hdcMemory;
	HBITMAP m_bitmap;
	int m_Width;
	int m_Heigh;
	CListCtrl m_ListMatrix;
	CImageList m_cImageListSmall;
	CString m_TextWord;
	CString m_EditHex;
	
};
