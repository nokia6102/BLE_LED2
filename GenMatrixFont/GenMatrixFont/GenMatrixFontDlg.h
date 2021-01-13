
// GenMatrixFontDlg.h : 標頭檔
//

#pragma once
#include "afxcmn.h"


// CGenMatrixFontDlg 對話方塊
class CGenMatrixFontDlg : public CDialogEx
{
// 建構
public:
	CGenMatrixFontDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
	enum { IDD = IDD_GENMATRIXFONT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援

private:
	void InitFont();
	void DrawFont();
	void ReleaseObj();
	void InitListCols();
	void InsertListItems(const HDC& hdc);
	void InsertColumns();

// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
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
