// ReplaceDll.cpp : Определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include "ReplaceDll.h"

using namespace std;

//функция замены строки 
void Replace(LPCSTR target, LPCSTR newStr) {
	
	string targetstr(target);
	string newstr(newStr);

	HANDLE hProcess = GetCurrentProcess();
	SYSTEM_INFO systemInfo;
	//получение информации о процессе
	GetSystemInfo(&systemInfo);
	//диапазон адресов виртуальной памяти
	DWORD_PTR minAddress = (DWORD_PTR)systemInfo.lpMinimumApplicationAddress;
	DWORD_PTR maxAddress = (DWORD_PTR)systemInfo.lpMaximumApplicationAddress;
	//буффер памяти размером в одну страницу 4кБ
	CHAR haystack[4096];
	MEMORY_BASIC_INFORMATION mbi;

	//проход по всей памяти
	for (DWORD_PTR addr = minAddress; addr < maxAddress; addr += systemInfo.dwPageSize)
	{
		//получение информации о стрнице памяти
		VirtualQueryEx(hProcess, (LPCVOID)(addr), &mbi, sizeof(mbi));

		if (mbi.State == MEM_COMMIT && mbi.Type == MEM_IMAGE && mbi.Protect == PAGE_READWRITE)
		{
			ZeroMemory(haystack, sizeof(haystack));
			//чтение страницы памяти в буффер 
			ReadProcessMemory(hProcess, (LPVOID)(addr), (LPVOID)haystack, sizeof(haystack), 0);
			//поиск вхождения строки в буфере
			CHAR *match = search(begin(haystack), end(haystack), targetstr.begin(), targetstr.end());
			//было ли вхождение
			BOOL isMatch = BOOL(match < end(haystack));
			//для вех вхождений
			while (match < end(haystack))
			{
				//изменение строки в буфере
				if (newstr.length() > targetstr.length())
					copy_n(newstr.begin(), targetstr.length(), match);
				else { //если новая строка меньше исходной
					match[newstr.length] = 0;
					copy(newstr.begin(), newstr.end(), match);
				}
				//поиск следующего вхождения
				match = search(match + targetstr.length(), end(haystack), targetstr.begin(), targetstr.end());
			}

			if (isMatch) //запись обновлённого буфера в память
				WriteProcessMemory(hProcess, (LPVOID)addr, (LPCVOID)haystack, sizeof(haystack), NULL);
		}
	}
}
