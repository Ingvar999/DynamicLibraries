// ReplaceDll.cpp : Определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include "ReplaceDll.h"

using namespace std;

void Replace(LPCSTR target, LPCSTR newStr) {
	
	string targetstr(target);
	string newstr(newStr);

	HANDLE hProcess = GetCurrentProcess();
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	DWORD_PTR minAddress = (DWORD_PTR)systemInfo.lpMinimumApplicationAddress;
	DWORD_PTR maxAddress = (DWORD_PTR)systemInfo.lpMaximumApplicationAddress;
	CHAR haystack[4096];
	MEMORY_BASIC_INFORMATION mbi;

	for (DWORD_PTR addr = minAddress; addr < maxAddress; addr += systemInfo.dwPageSize)
	{
		VirtualQueryEx(hProcess, (LPCVOID)(addr), &mbi, sizeof(mbi));

		if (mbi.State == MEM_COMMIT && mbi.Type == MEM_IMAGE && mbi.Protect == PAGE_READWRITE)
		{
			ZeroMemory(haystack, sizeof(haystack));
			ReadProcessMemory(hProcess, (LPVOID)(addr), (LPVOID)haystack, sizeof(haystack), 0);

			CHAR *match = search(begin(haystack), end(haystack), targetstr.begin(), targetstr.end());
			BOOL isMatch = BOOL(match < end(haystack));

			while (match < end(haystack))
			{
				if (newstr.length() > targetstr.length())
					copy_n(newstr.begin(), targetstr.length(), match);
				else
					copy(newstr.begin(), newstr.end(), match);

				match = search(match + targetstr.length(), end(haystack), targetstr.begin(), targetstr.end());
			}

			if (isMatch)
				WriteProcessMemory(hProcess, (LPVOID)addr, (LPCVOID)haystack, sizeof(haystack), NULL);
		}
	}
}
