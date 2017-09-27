
// AspirinAppDlg.h : header file
//

#pragma once


// CAspirinAppDlg dialog
class CAspirinAppDlg : public CDialog
{
	// Construction
public:
	explicit CAspirinAppDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	enum { IDD = IDD_ASPIRINAPP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	afx_msg void onNotifyKillFocus(UINT id);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedInboundComplete();
	afx_msg void OnBnClickedOutboundComplete();
	afx_msg void OnBnClickedRequestComplete();
};
