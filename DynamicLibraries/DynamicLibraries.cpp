// DynamicLibraries.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <conio.h>

//статическое подключение библиотек
#pragma comment(lib, "..\\Debug\\MyDll.lib")
#include "..\\MyDll\MyDll.h"

#pragma comment(lib, "..\\Debug\\ReplaceDll.lib")
#include "..\\ReplaceDll\ReplaceDll.h"

using namespace std;

typedef int Func(int, int);
DWORD GetProcessIdByName(const wstring &processName);
//название удалённого процесса
const wstring victimProcessName(L"VictimProcess.exe");

//тестовые строки для замены
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
	//статическое подключение библиотеки
	cout << "static includes" << endl << "min " << Min(5, 4) << endl << "max " << Max(5, 4) << endl;
	//динамичесоке подлючение библиотеки
	HMODULE lib = 0;
	if ((lib = LoadLibraryA("MyDll"))) {
		Func *_min, *_max;
		_min = (Func*)GetProcAddress(lib, MIN);
		_max = (Func*)GetProcAddress(lib, MAX);
		cout << "dynamic includes" << endl << "min " << _min(5, 4) << endl << "max " << _max(5, 4) << endl;
		FreeLibrary(lib);
	}
	//вывод строк до замены
	cout << "Before:" << endl;
	for (int i = 0; i < n; ++i)
		cout << strings[i].c_str() << endl;
	//замена строки функцией из библиотеки
	Replace(strings[3].c_str(), "Hello!!!");
	//вывод строк после замены
	cout << "After:" << endl;
	for (int i = 0; i < n; ++i)
		cout << strings[i].c_str() << endl;

	cout << "press any key to replace a string in other process" << endl;
	_getch();
	//получение дескриптора на удалённый процесс по его имени
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetProcessIdByName(victimProcessName));
	//получение указателя на функцию, которая будет выполняться в потоке удалённого процесса
	LPVOID threadFunction = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	//параметр для передачи в удалённый проект
	string parameter("InjectionDll.dll");
	//выделение памяти в удалённом процессе для размещения там параметра потока
	LPVOID argAddress = VirtualAllocEx(hRemoteProcess, NULL, parameter.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	SIZE_T k;
	//запись параметра в память удалённого процесса
	WriteProcessMemory(hRemoteProcess, argAddress, parameter.c_str(), parameter.length() + 1, &k);
	//создание удалённого потока с параметром
	CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)threadFunction, argAddress, NULL, NULL);

	CloseHandle(hRemoteProcess);

	return 0;
}

//определение id запущенного процесса по его имени 
DWORD GetProcessIdByName(const wstring &processName)
{
	DWORD processId = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 processEntry;
	ZeroMemory(&processEntry, sizeof(processEntry));
	processEntry.dwSize = sizeof(processEntry);

	bool isFound = false;
	// проход по всем процессам и проверка на совпадение имени
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