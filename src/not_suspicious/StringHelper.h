#ifndef _STRING_HELPER_H_
#define _STRING_HELPER_H_

namespace string_heper
{

std::wstring ToLower(std::wstring wstr);
std::string ToLower(std::string str);

bool FindSubstringW(std::wstring &wsStr, PCWSTR wszSubstr);
bool FindSubstringA(std::string &sStr, PCSTR szSubstr);

bool StartsWith(std::wstring &str, PCWSTR substr);
bool StartsWith(std::string &str, PCSTR substr);

}

#endif // _STRING_HELPER_H_
