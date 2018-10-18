// DynamicLibraries.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <conio.h>

#pragma comment(lib, "..\\Debug\\MyDll.lib")
#include "..\\MyDll\MyDll.h"

#pragma comment(lib, "..\\Debug\\ReplaceDll.lib")
#include "..\\ReplaceDll\ReplaceDll.h"

using namespace std;

typedef int Func(int, int);
DWORD GetProcessIdByName(const wstring &processName);
const wstring victimProcessName(L"VictimProcess.exe");

const int n = 10;
const string strings[n] = {
	"string 1",
	"string 2",
	"string 3",
	"string 4",
	"string 5",
	"string 6",
	"string 7",
	"string 8",
	"string 9",
	"string 10",
};

int main()
{
	cout << "static includes" << endl << "min " << Min(5, 4) << endl << "max " << Max(5, 4) << endl;
	HMODULE lib = 0;
	if ((lib = LoadLibraryA("MyDll"))) {
		Func *_min, *_max;
		_min = (Func*)GetProcAddress(lib, MIN);
		_max = (Func*)GetProcAddress(lib, MAX);
		cout << "dynamic includes" << endl << "min " << _min(5, 4) << endl << "max " << _max(5, 4) << endl;
		FreeLibrary(lib);
	}

	cout << "Before:" << endl;
	for (int i = 0; i < n; ++i)
		cout << strings[i].c_str() << endl;
	Replace(strings[3].c_str(), "Hello!!!");
	cout << "After:" << endl;
	for (int i = 0; i < n; ++i)
		cout << strings[i].c_str() << endl;

	cout << "press any key to replace a string in other process" << endl;
	_getch();

	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetProcessIdByName(victimProcessName));

	LPVOID threadFunction = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

	string parameter("InjectionDll.dll");
	LPVOID argAddress = VirtualAllocEx(hRemoteProcess, NULL, parameter.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	SIZE_T k;
	WriteProcessMemory(hRemoteProcess, argAddress, parameter.c_str(), parameter.length() + 1, &k);

	CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)threadFunction, argAddress, NULL, NULL);

	CloseHandle(hRemoteProcess);

	return 0;
}

DWORD GetProcessIdByName(const wstring &processName)
{
	DWORD processId = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 processEntry;
	ZeroMemory(&processEntry, sizeof(processEntry));
	processEntry.dwSize = sizeof(processEntry);

	bool isFound = false;
	while (Process32Next(hSnapshot, &processEntry) && !isFound)
	{
		if (!processName.compare(processEntry.szExeFile))
		{
			processId = processEntry.th32ProcessID;
			isFound = true;
		}
	}
	
	return processId;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
