// MyDll.cpp : Определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include "MyDll.h"

int Min(int a, int b) {
	if (a > b) 
		return b;
	else
		return a;
}

int Max(int a, int b) {
	if (a < b) 
		return b;
	else
		return a;
}