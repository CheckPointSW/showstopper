#include <string>
#include <algorithm>
#include <Windows.h>

#include "StringHelper.h"

std::wstring string_heper::ToLower(std::wstring wstr)
{
	std::transform(wstr.begin(), wstr.end(), wstr.begin(), [](wchar_t c)
	{ 
		return std::tolower(c); 
	});
	return wstr;
}

std::string string_heper::ToLower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](char c)
	{ 
		return std::tolower(c); 
	});
	return str;
}

bool string_heper::FindSubstringW(std::wstring &wsStr, PCWSTR wszSubstr)
{
	return wsStr.find(wszSubstr) != std::wstring::npos;
}

bool string_heper::FindSubstringA(std::string &sStr, PCSTR szSubstr)
{
	return sStr.find(szSubstr) != std::string::npos;
}

bool string_heper::StartsWith(std::wstring &wstr, PCWSTR substr)
{
	return wstr.find(substr) == 0;
}

bool string_heper::StartsWith(std::string &str, PCSTR substr)
{
	return str.find(substr) == 0;
}
