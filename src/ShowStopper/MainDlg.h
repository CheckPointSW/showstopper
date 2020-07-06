// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlwin.h>
#include <comutil.h>
#include <string>
#include <list>
#include <algorithm>
#include <sstream>
#include <codecvt>
#include <set>
#include <filesystem>
#include <memory>
#include <ctime>
#include <boost/algorithm/string/join.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "resource.h"
#include "ui_defs.h"
#include "config.h"
#include "interprocess.h"

#pragma comment(lib, "comsuppw.lib")

namespace pt = boost::property_tree;

class CMainDlg : 
	public CDialogImpl<CMainDlg>, 
	public CUpdateUI<CMainDlg>,
	public CMessageFilter, 
	public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG *pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateChildWindows();
		return FALSE;
	}

	void CloseDialog(int nVal)
	{
		DestroyWindow();
		::PostQuitMessage(nVal);
	}

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_UPDATE_ADDRESS, OnUpdateAddress)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_GENERATE, OnGenerate)
		COMMAND_ID_HANDLER(IDC_GENNRUN, OnGenerateNRun)
		COMMAND_ID_HANDLER(IDC_ABOUT, OnAbout)
		COMMAND_CODE_HANDLER(CBN_SELENDOK, OnComboChange)
		NOTIFY_HANDLER(m_nHeaderId, HDN_ITEMSTATEICONCLICK, OnHeaderItemStateIconClick)
		NOTIFY_HANDLER(IDC_LIST1, LVN_ITEMCHANGED, OnListItemChanged)
		NOTIFY_HANDLER(IDC_LIST1, NM_DBLCLK, OnTableRowClick)
		NOTIFY_HANDLER(IDC_LIST1, NM_RCLICK, OnTableRowRightClick)
		NOTIFY_HANDLER(IDC_LIST1, LVN_GETINFOTIP, OnTableRowGetToolTip)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(hIconSmall, FALSE);

		// register object for message filtering and idle updates
		CMessageLoop *pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		UIAddChildWindowContainer(m_hWnd);

		// Init Ui Components
		InitTechniques();
		InitTechniquesTable();
		InitTechniquesTableGroups();
		InitComboBox();

		// Load default config
		if (LoadConfigFromResource())
			ParseConfig();

		// Set checkboxes in table for default mode
		EditTableByComboIndex(0);
		// Grab tags of checked techniques in table
		ParseTable();

		m_SharedFile = { 0 };

		return TRUE;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (interprocess::IsSharedFileOpened(&m_SharedFile))
			interprocess::CloseSharedFile(&m_SharedFile);

		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CloseDialog(wID);
		return 0;
	}

	LRESULT OnUpdateAddress(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		int param = (int)lParam;
		WCHAR addr[10];
		wsprintf(addr, L"%08X", param);
		m_lvTable.SetItemText((int)wParam, Columns::TechniqueAddress, addr);

		if (interprocess::IsSharedFileOpened(&m_SharedFile))
			interprocess::CloseSharedFile(&m_SharedFile);

		return 0;
	}

	LRESULT OnGenerate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ParseTable();

		std::string sNewConfig = GenerateConfig();
		if (!sNewConfig.empty())
		{
			CFileDialog dlg(FALSE, L"json", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"JSON Files (*.json)\0*.json\0\0");
			int nRet = dlg.DoModal();

			if(nRet == IDOK)
			{
				ATLTRACE(_T("File path: %s\n"), dlg.m_ofn.lpstrFile);
				BOOL bRet = SaveFile(dlg.m_ofn.lpstrFile, sNewConfig.c_str(), sNewConfig.size());
				if (!bRet)
					MessageBox(_T("Error writing file!\n"));
			}
		}

		return 0;
	}

	LRESULT OnGenerateNRun(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ParseTable();
		ClearCheckAddresses();

		CHAR szSharedFileName[MAX_PATH];
		std::srand((uint32_t)std::time(nullptr));
		sprintf_s(szSharedFileName, "%s%d.json", "OwllySharedConfig", std::rand());

		std::string sNewConfig = GenerateConfig();

		if (interprocess::IsSharedFileOpened(&m_SharedFile))
			interprocess::CloseSharedFile(&m_SharedFile);

		if (!interprocess::InitSharedFile(&m_SharedFile, szSharedFileName, strlen(szSharedFileName), sNewConfig.size()))
			return MessageBoxA(m_hWnd, "Can not initialize shared file data!", "Owlly Error", MB_OK | MB_ICONERROR);

		if (!interprocess::WriteSharedFile(&m_SharedFile, (PSTR)sNewConfig.c_str(), sNewConfig.size()))
			return MessageBoxA(m_hWnd, "Can not write config to a shared file!", "Owlly Error", MB_OK | MB_ICONERROR);

		std::wstring wsCmdArgs = (m_sCheckedTags.size() > 0)
			? L" --" + boost::algorithm::join(m_sCheckedTags, L" --")
			: L"";
		wsCmdArgs += L" --auxiliary UiProxy," + std::to_wstring((DWORD)m_hWnd) + L"," + s2ws(szSharedFileName) + L"," + std::to_wstring(sNewConfig.size());

		CHAR szFilePath[MAX_PATH];
		if (NULL == GetModuleFileNameA(NULL, szFilePath, MAX_PATH))
			return MessageBoxA(m_hWnd, "Can not retrive current path!", "Owlly Error", MB_OK | MB_ICONERROR);

		std::filesystem::path exePath = szFilePath;
		auto cmdLine = exePath.parent_path().append("not_suspicious.exe");
		if (!std::filesystem::exists(cmdLine))
			return MessageBoxA(m_hWnd, "Can not file not_suspicious.exe!", "Owlly Error", MB_OK | MB_ICONERROR);

		cmdLine += wsCmdArgs;

		STARTUPINFO si = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION pi;
		CreateProcessW(NULL, (LPWSTR)cmdLine.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
		return 0;
	}

	LRESULT OnAbout(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnComboChange(WORD, WORD, HWND, BOOL&)
	{
		for (int i = 0; i < m_lvTable.GetItemCount(); i++)
			m_lvTable.SetCheckState(i, FALSE);

		EditTableByComboIndex(m_ComboBox.GetCurSel());

		return 0;
	}

	LRESULT OnHeaderItemStateIconClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		LPNMHEADER pnmHeader = (LPNMHEADER)pnmh;
		if (pnmHeader->pitem->mask & HDI_FORMAT && 
			pnmHeader->pitem->fmt & HDF_CHECKBOX)
		{
			CheckAllItems(!(pnmHeader->pitem->fmt & HDF_CHECKED));
			SetHeaderCheckbox();
			return 1;
		}
		return 0;
	}

	LRESULT OnListItemChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		LPNMLISTVIEW pnmlv = (LPNMLISTVIEW)pnmh;
		if (pnmlv->uChanged & LVIF_STATE)
			SetHeaderCheckbox();
		return 0;
	}

	LRESULT OnTableRowClick(int idCtrl, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		int nSelectedIndex = m_lvTable.GetHotItem();
		auto it = Config::TechniqueToName.find((Config::ETechnique)nSelectedIndex);
		if (it != Config::TechniqueToName.end())
		{
			auto wsTechniqueName = s2ws(it->second);
			std::vector<Parameter> vParameters;

			auto itp = m_mParamMap.find(wsTechniqueName);
			if (itp != m_mParamMap.end())
				vParameters = itp->second;

			CInfoDlg dlg(wsTechniqueName, &vParameters);
			dlg.DoModal();

			for (auto new_param : vParameters)
			{
				for (auto &old_param : m_mParamMap[wsTechniqueName])
					old_param.Value = new_param.Value;
			}
		}
		return 0;
	}

	LRESULT OnTableRowRightClick(int idCtrl, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		int nSelectedIndex = m_lvTable.GetHotItem();
		BSTR bsAddress = nullptr;
		if (TRUE == m_lvTable.GetItemText(nSelectedIndex, Columns::TechniqueAddress, bsAddress))
		{
			PSTR pszAddress = _com_util::ConvertBSTRToString(bsAddress);
			size_t nAddrStrLen = strlen(pszAddress) + 1;

			OpenClipboard();
			EmptyClipboard();	
			HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, nAddrStrLen);
			if (!hg)
				CloseClipboard();
			else
			{
				memcpy(GlobalLock(hg), pszAddress, nAddrStrLen);
				GlobalUnlock(hg);
				SetClipboardData(CF_TEXT, hg);
				CloseClipboard();
				GlobalFree(hg);
			}
		}
		return 0;
	}

	LRESULT OnTableRowGetToolTip(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMLVGETINFOTIP pGetInfoTip = (LPNMLVGETINFOTIP)pnmh;
		if ((NULL != pGetInfoTip) && (m_mIdToInfo.find((Config::ETechnique)pGetInfoTip->iItem) != m_mIdToInfo.end()))
		{
			memset(pGetInfoTip->pszText, 0, pGetInfoTip->cchTextMax);

			auto &wsToolTip = m_mIdToInfo[(Config::ETechnique)pGetInfoTip->iItem];
			memcpy_s(pGetInfoTip->pszText, pGetInfoTip->cchTextMax, wsToolTip.c_str(), wsToolTip.size() * 2);
		}
		return S_OK;
	}

//
// Init UI Functions
//

	void InitTechniques()
	{
		m_mTechniqueToId.clear();
		for (auto &[id, name] : Config::TechniqueToName)
			m_mTechniqueToId[name] = id;
	}

	void InitTechniquesTable()
	{
		m_lvTable.Attach(GetDlgItem(IDC_LIST1));
		m_lvTable.SetExtendedListViewStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_LABELTIP, 0);

		LVCOLUMN lvc = { 0 };

		m_lvTable.InsertColumn(Columns::CheckBox, &lvc);
		lvc.mask = LVCF_TEXT;

		lvc.iSubItem++;
		lvc.pszText = (LPWSTR)g_aColumnNames[Columns::TechniqueName];
		m_lvTable.InsertColumn(Columns::TechniqueName, &lvc);

		lvc.iSubItem++;
		lvc.pszText = (LPWSTR)g_aColumnNames[Columns::TechniqueAddress];;
		m_lvTable.InsertColumn(Columns::TechniqueAddress, &lvc);

		lvc.iSubItem++;
		lvc.pszText = (LPWSTR)g_aColumnNames[Columns::TechniqueTags];
		m_lvTable.InsertColumn(Columns::TechniqueTags, &lvc);

		m_lvTable.SetColumnWidth(Columns::CheckBox, LVSCW_AUTOSIZE_USEHEADER);
		m_lvTable.SetColumnWidth(Columns::TechniqueName, 350);
		m_lvTable.SetColumnWidth(Columns::TechniqueAddress, LVSCW_AUTOSIZE_USEHEADER);
		m_lvTable.SetColumnWidth(Columns::TechniqueTags, 250);

		CHeaderCtrl headerCtl = m_lvTable.GetHeader();
		LONG dwHeaderStyle = headerCtl.GetWindowLongW(GWL_STYLE);
		dwHeaderStyle |= HDS_CHECKBOXES;
		headerCtl.SetWindowLongW(GWL_STYLE, dwHeaderStyle);

		m_nHeaderId = headerCtl.GetDlgCtrlID();

		HDITEM hdi = { 0 };
		hdi.mask = HDI_FORMAT;
		headerCtl.GetItem(0, &hdi);
		hdi.fmt |= HDF_CHECKBOX | HDF_FIXEDWIDTH;
		headerCtl.SetItem(0, &hdi);
	}

	void InitTechniquesTableGroups()
	{
		LVGROUP group;
		group.cbSize    = sizeof(LVGROUP);
		group.mask      = LVGF_HEADER | LVGF_GROUPID;

		m_lvTable.EnableGroupView(TRUE);
		for (size_t i = 0; i < g_vGroups.size(); i++)
		{
			group.pszHeader = (LPWSTR)g_vGroups[i].c_str();
			group.iGroupId  = i;

			int gid = m_lvTable.AddGroup(&group);
			OutputDebugStringA(std::to_string(gid).c_str());
		}
	}

	void InitComboBox()
	{
		m_ComboBox.Attach(GetDlgItem(IDC_COMBO1));
		m_ComboBox.InsertString(Basic, L"Basic");
		m_ComboBox.InsertString(AllInteractive, L"All Interactive");
		m_ComboBox.InsertString(AllAntiTrace, L"All Anti-Trace");
		m_ComboBox.InsertString(AllAntiBreakpoint, L"All Anti-Breakpoint");
		m_ComboBox.InsertString(AllAntiAttach, L"All Anti-Attach");
		m_ComboBox.InsertString(AllCrashCausing, L"All That Cause Crash");
		m_ComboBox.InsertString(AllAdminPrriv, L"All That Need Admin Rights");
		m_ComboBox.InsertString(Group_Flags_WinApi, L"Group:DebugFlags:WinAPI");
		m_ComboBox.InsertString(Group_Flags_Manual, L"Group:DebugFlags:Manual");
		m_ComboBox.InsertString(Group_Handles, L"Group:Handles");
		m_ComboBox.InsertString(Group_Exceptions, L"Group:Exceptions");
		m_ComboBox.InsertString(Group_Timing, L"Group:Timing");
		m_ComboBox.InsertString(Group_ProcMem_Bp, L"Group:Memory:Anti-Breakpoints");
		m_ComboBox.InsertString(Group_ProcMem_Other, L"Group:Memory:Other");
		m_ComboBox.InsertString(Group_Asm, L"Group:CPU");
		m_ComboBox.InsertString(Group_DbgInteraction, L"Group:Dbg-Interaction");
		m_ComboBox.InsertString(Group_Misc, L"Group:Misc");
		m_ComboBox.SetCurSel(0);
	}

	bool LoadConfigFromResource()
	{
		HMODULE hModule = GetModuleHandle(NULL);
		if (hModule)
		{
			HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(IDR_CONFIG), MAKEINTRESOURCE(TEXTFILE));
			if (hResource)
			{
				HGLOBAL hData = LoadResource(hModule, hResource);
				if (hData)
				{
					m_sConfigData.assign({ (PSTR)LockResource(hData) });
					return true;
				}
			}
		}
		return false;
	}

	bool ParseConfig()
	{
		namespace pt = boost::property_tree;
		std::string sParentChain;

		try
		{
			std::stringstream ssConfig;
			ssConfig << m_sConfigData;

			pt::ptree root;
			pt::read_json(ssConfig, root);

			for (auto &node : root)
			{
				auto name = node.second.get<std::string>("name");
				auto subgroups = node.second.get_child_optional("subgroups");
				if (!subgroups)
					continue;

				sParentChain.clear();
				sParentChain += "[";
				sParentChain += name;
				sParentChain += "]";
				ParseNode(subgroups.get(), sParentChain);

				sParentChain.clear();
			}
		}
		catch (const pt::ptree_error &e)
		{
			OutputDebugStringA(e.what());
			return false;
		}
		catch (...)
		{
			OutputDebugStringA("Unknown error while parsing the config.");
			return false;
		}
		return true;
	}

	void ParseNode(boost::property_tree::ptree &root, std::string parent)
	{
		for (auto &node : root)
		{
			auto name = node.second.get<std::string>("name");
			auto subgroups = node.second.get_child_optional("subgroups");
			auto params = node.second.get_child_optional("parameters");
			if (params)
			{
				auto wsName = s2ws(name);
				for (auto &param : params.get())
				{
					auto param_name = param.second.get<std::string>("name");
					auto param_type = param.second.get<std::string>("type");
					auto param_value = param.second.get<std::string>("value");

					Parameter parameter {
						s2ws(param_name),
						s2ws(param_type),
						s2ws(param_value),
					};
					
					if (m_mParamMap.find(wsName) == m_mParamMap.end())
						m_mParamMap.insert(std::make_pair(wsName, std::vector<Parameter>{parameter}));
					else
						m_mParamMap[wsName].push_back(parameter);
				}
			}

			std::list<std::wstring> tags;
			if (node.second.get_child_optional("tags"))
			{
				for (boost::property_tree::ptree::value_type &tag : node.second.get_child("tags"))
					tags.push_back(s2ws(tag.second.get<std::string>("", "")));
			}

			if (!subgroups)
			{
				std::wstring wsTechniqueName = s2ws(name);

				int group = 0;
				auto it = std::find(g_vGroups.begin(), g_vGroups.end(), s2ws(parent));
				if (it != g_vGroups.cend())
					group = std::distance(g_vGroups.begin(), it);

				int row = AddTechnique(group, wsTechniqueName, 0, tags);
				assert(row == (int)m_mTechniqueToId[name]);

				if (node.second.get_child_optional("info"))
					m_mIdToInfo[(Config::ETechnique)row] = s2ws(node.second.get<std::string>("info"));

				for (auto tag : tags)
				{
					if (m_mTagToIndexes.find(tag) == m_mTagToIndexes.end())
						m_mTagToIndexes[tag] = std::vector<int>{ row };
					else
						m_mTagToIndexes[tag].push_back(row);
				}

				continue;
			}
			else
			{
				ParseNode(subgroups.get(), parent + "[" + name + "]");
			}
		}
	}

	void EditTableByComboIndex(int cbIndex)
	{
		switch (cbIndex)
		{
		case Basic:
		{
			for (int i = 0; i < m_lvTable.GetItemCount(); i++)
			{
				if (!IsTagged(i, L"crash") && !IsTagged(i, L"admin"))
					m_lvTable.SetCheckState(i, TRUE);
			}
			break;
		}
		case AllInteractive:
		{
			CheckRowsByTag(L"trace");
			CheckRowsByTag(L"break");
			CheckRowsByTag(L"attach");
			break;
		}
		case AllAntiTrace:
		{
			CheckRowsByTag(L"trace");
			break;
		}
		case AllAntiBreakpoint:
		{
			CheckRowsByTag(L"break");
			break;
		}
		case AllAntiAttach:
		{
			CheckRowsByTag(L"attach");
			break;
		}
		case AllCrashCausing:
		{
			CheckRowsByTag(L"crash");
			break;
		}
		case AllAdminPrriv:
		{
			CheckRowsByTag(L"admin");
			break;
		}
		default:
		{
			int group_id = cbIndex - Group_Flags_WinApi;
			if (group_id >= 0)
			{
				CheckRowsByGroup((Group)group_id);
			}
			break;
		}
		}
	}

	void ParseTable()
	{
		assert(m_lvTable.GetItemCount() == m_mTechniqueToId.size());

		m_sCheckedTechniques.clear();
		m_sCheckedTags.clear();
		for (auto &[name, id] : m_mTechniqueToId)
		{
			if (FALSE == m_lvTable.GetCheckState((int)id))
				continue;
			m_sCheckedTechniques.insert(name);

			for (auto &[tag, vIndexes] : m_mTagToIndexes)
			{
				if (std::find(vIndexes.begin(), vIndexes.end(), (int)id) != vIndexes.end())
					m_sCheckedTags.insert(tag);
			}
		}
	}

//
// Handlers for checkboxes in list view
//

	void CheckAllItems(BOOL bChecked)
	{
		for (int i = 0; i < m_lvTable.GetItemCount(); i++)
			m_lvTable.SetCheckState(i, bChecked);
	}

	void SetHeaderCheckbox(void)
	{
		BOOL bChecked = TRUE;
		for (int i = 0; i < m_lvTable.GetItemCount(); i++)
		{
			if (!m_lvTable.GetCheckState(i))
			{
				bChecked = FALSE;
				break;
			}
		}

		CHeaderCtrl header = m_lvTable.GetHeader();
		HDITEM hdi = { 0 };
		hdi.mask = HDI_FORMAT;

		header.GetItem(0, &hdi);
		if (bChecked)
			hdi.fmt |= HDF_CHECKED;
		else
			hdi.fmt &= ~HDF_CHECKED;

		header.SetItem(0, &hdi);
	}

//
// Methods for techniques & list view parsing
//

	void CheckRowsByTag(PCWSTR wszTag)
	{
		if (m_mTagToIndexes.find(wszTag) != m_mTagToIndexes.end())
		{
			for (int idx : m_mTagToIndexes[wszTag])
				m_lvTable.SetCheckState(idx, TRUE);
		}
	}

	void CheckRowsByGroup(Group gid)
	{
		LVGROUP group = { 0 };
		group.cbSize = sizeof(LVGROUP);
		group.mask = LVGF_ITEMS;
		if (-1 != m_lvTable.GetGroupInfo((int)gid, &group))
		{
			for (size_t i = group.iFirstItem; i < group.iFirstItem + group.cItems; i++)
				m_lvTable.SetCheckState(i, TRUE);
		}
	}

	bool IsTagged(int idx, LPCWSTR wszTag)
	{
		if (m_mTagToIndexes.find(wszTag) == m_mTagToIndexes.end())
			return false;

		auto it = m_mTagToIndexes[wszTag];
		return std::find(it.begin(), it.end(), idx) != it.end();
	}
	
	void ClearCheckAddresses()
	{
		for (int i = 0; i < m_lvTable.GetItemCount(); i++)
			m_lvTable.SetItemText(i, Columns::TechniqueAddress, L"Unknown");
	}

	int AddTechnique(int group, std::wstring wsName, DWORD dwAddress, std::list<std::wstring> lstTags)
	{
		LVITEM lvi = { 0 };
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE | LVIF_GROUPID; 
		lvi.state = 0;
		lvi.stateMask = 0;
		lvi.iItem = m_lvTable.GetItemCount();
		lvi.iGroupId = group;

		WCHAR wszAddress[MAX_PATH];
		if (0 != dwAddress)
			wsprintf(wszAddress, L"%08X", dwAddress);
		else
			wsprintf(wszAddress, L"Unknown");

		std::wstring wsTags = boost::algorithm::join(lstTags, L", ");
		
		int row = m_lvTable.InsertItem(&lvi);
		m_lvTable.SetItemText(row, Columns::TechniqueName, (LPWSTR)wsName.c_str());
		m_lvTable.SetItemText(row, Columns::TechniqueAddress, wszAddress);
		m_lvTable.SetItemText(row, Columns::TechniqueTags, (LPWSTR)wsTags.c_str());
		return row;
	}

//
// Config generation
//

	void FilterNode(boost::property_tree::ptree &root, boost::property_tree::ptree &child_subgroups)
	{
		for (auto &node : root)
		{
			boost::property_tree::ptree child;

			auto name = node.second.get<std::string>("name");
			auto subgroups = node.second.get_child_optional("subgroups");

			if (subgroups)
			{
				pt::ptree nested_child_subgroups;
				FilterNode(subgroups.get(), nested_child_subgroups);

				if (!nested_child_subgroups.empty())
				{
					child.put("name", name);
					child.add_child("subgroups", nested_child_subgroups);
				}

				if (child.get_child_optional("name"))
					child_subgroups.push_back(std::make_pair("", child));
			}
			else
			{
				if (m_sCheckedTechniques.find(name) == m_sCheckedTechniques.end())
					continue;

				child.put("name", name);

				auto tags = node.second.get_child_optional("tags");
				if (tags)
				{
					pt::ptree ptags;
					for (auto t : tags.get())
					{
						pt::ptree ptag;
						ptag.put("", t.second.get_value<std::string>());
						ptags.push_back(std::make_pair("", ptag));
					}
					child.add_child("tags", ptags);
				}

				auto params = node.second.get_child_optional("parameters");
				if (params)
				{
					pt::ptree pparams;
					for (auto p : params.get())
					{
						auto param_name = p.second.get_child("name").get_value<std::string>();
						auto param_type = p.second.get_child("type").get_value<std::string>();
						auto param_value = p.second.get_child("value").get_value<std::string>();
						
						auto it = m_mParamMap.find(s2ws(name));
						if (it != m_mParamMap.end())
						{
							for (auto &[n, t, v] : m_mParamMap[s2ws(name)])
							{
								if (s2ws(param_name) == n)
								{
									param_value = ws2s(v);
									break;
								}
							}
						}

						pt::ptree pparam;
						pparam.put("name", param_name);
						pparam.put("type", param_type);
						pparam.put("value", param_value);
						pparams.push_back(std::make_pair("", pparam));
					}
					child.add_child("parameters", pparams);
				}

				child_subgroups.push_back(std::make_pair("", child));
				continue;
			}
		}
	}

	std::string GenerateConfig()
	{
		try
		{
			pt::ptree new_list;

			std::stringstream ssConfig;
			ssConfig << m_sConfigData;

			pt::ptree root;
			pt::read_json(ssConfig, root);

			for (auto &node : root)
			{
				pt::ptree child;
				auto name = node.second.get<std::string>("name");
				auto subgroups = node.second.get_child_optional("subgroups");

				child.put("name", name);

				if (!subgroups)
					continue;

				pt::ptree child_subgroups;
				FilterNode(subgroups.get(), child_subgroups);

				if (!child_subgroups.empty())
					child.add_child("subgroups", child_subgroups);

				if (child.get_child_optional("subgroups"))
					new_list.push_back(std::make_pair("", child));
			}

			std::stringstream ss;
			pt::json_parser::write_json(ss, new_list);
			return ss.str();
		}
		catch (...)
		{
		}
		return "";
	}

//
// Other methods
//

	std::wstring s2ws(const std::string &str) const
	{
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;
		return converterX.from_bytes(str);
	}

	std::string ws2s(const std::wstring &wstr) const
	{
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;
		return converterX.to_bytes(wstr);
	}

	BOOL SaveFile(LPTSTR lpstrFilePath, LPCSTR szData, DWORD dwSize)
	{
		_ASSERTE(lpstrFilePath != NULL);

		HANDLE hFile = ::CreateFile(lpstrFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return FALSE;

		DWORD dwWritten;
		if (FALSE == WriteFile(hFile, szData, dwSize, &dwWritten, NULL))
			return FALSE;

		::CloseHandle(hFile);

		return TRUE;
	}

private:
	int m_nHeaderId;

	CListViewCtrl m_lvTable;
	CComboBox m_ComboBox;

	std::string m_sConfigData;
	std::set<std::string> m_sCheckedTechniques;
	std::set<std::wstring> m_sCheckedTags;

	std::map<std::string, Config::ETechnique> m_mTechniqueToId;
	std::map<std::wstring, std::vector<int>> m_mTagToIndexes;
	std::map<std::wstring, std::vector<Parameter>> m_mParamMap;
	std::map<Config::ETechnique, std::wstring> m_mIdToInfo;

	interprocess::SharedFile m_SharedFile;
};
