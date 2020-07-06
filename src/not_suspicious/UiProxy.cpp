#include "UiProxy.h"
#include <Windows.h>

#define LVM_FIRST               0x1000      // ListView messages
#define LVM_SETITEMTEXTW        (LVM_FIRST + 116)

typedef struct tagLVITEMW
{
    UINT mask;
    int iItem;
    int iSubItem;
    UINT state;
    UINT stateMask;
    LPWSTR pszText;
    int cchTextMax;
    int iImage;
    LPARAM lParam;
    int iIndent;
#if (NTDDI_VERSION >= NTDDI_WINXP)
    int iGroupId;
    UINT cColumns; // tile view columns
    PUINT puColumns;
#endif
#if (NTDDI_VERSION >= NTDDI_VISTA)
    int* piColFmt;
    int iGroup; // readonly. only valid for owner data.
#endif
} LVITEMW, *LPLVITEMW;

UiProxy::UiProxy()
	: hListView(nullptr)
	, bEnabled(false)
    , szFileName{0}
    , dwFileSize(0)
{
}

UiProxy &UiProxy::GetInstance()
{
	static UiProxy UiProxy;
	return UiProxy;
}

int UiProxy::GetTechniqueId(std::string &sTechniqueName)
{
	if (m_mNameToId.empty())
	{
		for (auto &[id, name] : Config::TechniqueToName)
			m_mNameToId[name] = (int)id;
	}
	return m_mNameToId[sTechniqueName];
}
#define CUSTOM_WM_MESSAGE (WM_APP + 1)

void UiProxy::SetCheckAddress(int nIndex, unsigned long dwAddress)
{
    SendMessageW((HWND)hListView, CUSTOM_WM_MESSAGE, (WPARAM)nIndex, (LPARAM)dwAddress);
}
