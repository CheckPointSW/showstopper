#ifndef _UI_PROXY_H_
#define _UI_PROXY_H_

#include "config.h"

struct UiProxy
{
	void *hListView;
	bool bEnabled;

	char szFileName[256];
	unsigned long dwFileSize;

	UiProxy(const UiProxy &) = delete;
	void operator=(const UiProxy &) = delete;

	static UiProxy &GetInstance();
	
	int GetTechniqueId(std::string &sTechniqueName);
	void SetCheckAddress(int nIndex, unsigned long dwAddress);

private:
	UiProxy();

	std::map<std::string, int> m_mNameToId;
};

#endif // _UI_PROXY_H_
