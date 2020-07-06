#ifndef _SHARED_FILE_H_
#define _SHARED_FILE_H_

#include <Windows.h>

namespace interprocess
{
	struct SharedFile
	{
		HANDLE hMapFile;
		LPVOID pBuffer;

		CHAR szFileName[MAX_PATH];
		DWORD dwFileSize;
	};

	bool IsSharedFileOpened(SharedFile *pSharedFile)
	{
		return pSharedFile->szFileName
			&& pSharedFile->hMapFile
			&& pSharedFile->pBuffer;
	}

	bool InitSharedFile(SharedFile *pSharedFile, PCSTR szFileName, DWORD dwFileNameLength, DWORD dwFileSize)
	{
		if (!pSharedFile || !szFileName || !dwFileNameLength || !dwFileSize)
			return false;

		pSharedFile->dwFileSize = dwFileSize;
		pSharedFile->hMapFile = NULL;
		pSharedFile->pBuffer = NULL;
		ZeroMemory(pSharedFile->szFileName, MAX_PATH);
		return 0 == ::memcpy_s(pSharedFile->szFileName, MAX_PATH, szFileName, dwFileNameLength);
	}

	bool WriteSharedFile(SharedFile *pSharedFile, PSTR pData, DWORD dwSize)
	{
		if (!pSharedFile || !pData || !dwSize)
			return false;
		if (!pSharedFile->szFileName || !pSharedFile->dwFileSize)
			return false;

		pSharedFile->dwFileSize = dwSize;
		pSharedFile->hMapFile = CreateFileMappingA(
			INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE,
			0,
			dwSize,
			pSharedFile->szFileName);
		if (NULL == pSharedFile->hMapFile)
			return false;

		pSharedFile->pBuffer = MapViewOfFile(
			pSharedFile->hMapFile,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			pSharedFile->dwFileSize);
		if (NULL == pSharedFile->pBuffer)
		{
			CloseHandle(pSharedFile->hMapFile);
			return false;
		}

		return 0 == ::memcpy_s(
			pSharedFile->pBuffer, pSharedFile->dwFileSize * sizeof(CHAR),
			pData, pSharedFile->dwFileSize * sizeof(CHAR));
	}

	bool ReadSharedFile(SharedFile *pSharedFile)
	{
		if (!pSharedFile || !pSharedFile->szFileName || !pSharedFile->dwFileSize)
			return false;
		if (pSharedFile->pBuffer || pSharedFile->hMapFile)
			return false;

		pSharedFile->hMapFile = OpenFileMappingA(
			FILE_MAP_ALL_ACCESS,
			FALSE,
			pSharedFile->szFileName);
		if (NULL == pSharedFile->hMapFile)
			return false;

		pSharedFile->pBuffer = MapViewOfFile(
			pSharedFile->hMapFile,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			pSharedFile->dwFileSize);
		if (NULL == pSharedFile->pBuffer)
		{
			CloseHandle(pSharedFile->hMapFile);
			return false;
		}

		return true;
	}

	void CloseSharedFile(SharedFile *pSharedFile)
	{
		try
		{
			if (NULL != pSharedFile->pBuffer)
			{
				UnmapViewOfFile(pSharedFile->pBuffer);
				pSharedFile->pBuffer = NULL;
			}
			if (NULL != pSharedFile->hMapFile)
			{
				CloseHandle(pSharedFile->hMapFile);
				pSharedFile->hMapFile = NULL;
			}
			if (pSharedFile->szFileName)
				ZeroMemory(pSharedFile->szFileName, MAX_PATH);
		}
		catch (...)
		{
		}
	}
}

#endif // _SHARED_FILE_H_
