#pragma once

#include <atlwin.h>
#include "resource.h"

class CAboutDlg : public CDialogImpl<CAboutDlg>
{
public:
    enum { IDD = IDD_ABOUT };

    BEGIN_MSG_MAP(CAboutDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
    END_MSG_MAP()

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CenterWindow();
        SetWindowTextW(L"ShowStopper: About");

        m_stCopyrightText.Attach(GetDlgItem(IDC_COPYRIGHT));
        m_stCopyrightText.SetWindowTextW(L"(c) 2020 Check Point Software Technologies LTD\nProgrammed by Yaraslau Harakhavik");

        return TRUE;
    }

    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        EndDialog(IDCANCEL);
        return 0;
    }

    LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
    {
        EndDialog(wID);
        return 0;
    }

private:
    CStatic m_stCopyrightText;
};
