#pragma once

#include "ui_defs.h"
#include "resource.h"

class CInfoDlg : public CDialogImpl<CInfoDlg>
{
public:
	enum { IDD = IDD_INFODLG };

	BEGIN_MSG_MAP(CInfoDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(IDC_BUTTON_SET_VALUE, OnSetValue)
		COMMAND_CODE_HANDLER(CBN_SELENDOK, OnComboChange)
	END_MSG_MAP()

	CInfoDlg(std::wstring &wsTechnique, std::vector<Parameter> *params)
	{
		m_wsTechniqueName = wsTechnique;
		m_pvParameters = params;
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		SetWindowTextW(m_wsTechniqueName.c_str());

		m_eTechniqueName.Attach(GetDlgItem(IDC_EDIT_NAME));
		m_eTechniqueName.SetWindowTextW(m_wsTechniqueName.c_str());

		if (m_pvParameters->empty())
		{
			::ShowWindow(GetDlgItem(IDC_COMBO_NAME), SW_HIDE);
			::ShowWindow(GetDlgItem(IDC_EDIT_TYPE), SW_HIDE);
			::ShowWindow(GetDlgItem(IDC_EDIT_VALUE), SW_HIDE);
			::ShowWindow(GetDlgItem(IDC_STATIC_PNAME), SW_HIDE);
			::ShowWindow(GetDlgItem(IDC_STATIC_PTYPE), SW_HIDE);
			::ShowWindow(GetDlgItem(IDC_STATIC_PVALUE), SW_HIDE);
			::ShowWindow(GetDlgItem(IDC_BUTTON_SET_VALUE), SW_HIDE);
		}
		else
		{
			::ShowWindow(GetDlgItem(IDC_NO_PARAM_TEXT), SW_HIDE);
			
			m_cbParamNames.Attach(GetDlgItem(IDC_COMBO_NAME));
			for (size_t i = 0; i < m_pvParameters->size(); i++)
				m_cbParamNames.InsertString(i, (*m_pvParameters)[i].Name.c_str());
			m_cbParamNames.SetCurSel(0);
			FillParameterBox(0);
		}

		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnSetValue(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		WCHAR wszValue[MAX_PATH];
		::GetWindowTextW(GetDlgItem(IDC_EDIT_VALUE), (LPWSTR)&wszValue, MAX_PATH);

		int idx = m_cbParamNames.GetCurSel();
		auto parameter = (*m_pvParameters)[idx];

		std::wstring wsValue{ wszValue };
		if (parameter.Type == L"dword" || parameter.Type == L"qword")
		{
			if (!IsNumeric(wsValue))
				return MessageBox(L"Value must be numeric!");
		}
		else if (parameter.Type == L"real")
		{
			if (!IsReal(wsValue))
				return MessageBox(L"Value must be real number!");
		}

		(*m_pvParameters)[idx].Value = wsValue;
		MessageBox(L"New value has been saved!");
		return 0;
	}

	LRESULT OnComboChange(WORD, WORD, HWND, BOOL&)
	{
		int idx = m_cbParamNames.GetCurSel();
		FillParameterBox(idx);
		return 0;
	}

	void FillParameterBox(int idx)
	{
		auto parameter = (*m_pvParameters)[idx];
		::SetWindowTextW(GetDlgItem(IDC_EDIT_TYPE), parameter.Type.c_str());
		::SetWindowTextW(GetDlgItem(IDC_EDIT_VALUE), parameter.Value.c_str());
	}

	bool IsNumeric(std::wstring& wstr)
	{
		for (wchar_t c : wstr)
		{
			if (!iswdigit(c))
				return false;
		}
		return true;
	}

	bool IsReal(std::wstring& wstr)
	{
		for (wchar_t c : wstr)
		{
			if (!iswdigit(c) && (c != L'.'))
				return false;
		}
		return true;
	}

private:
	std::wstring m_wsTechniqueName;
	std::vector<Parameter> *m_pvParameters;
	
	CEdit m_eTechniqueName;
	CComboBox m_cbParamNames;
};
