// VictimProcess.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <conio.h>

using namespace std;

//тестовые строки
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
	//вывод текущего содержимого 
    cout << "press any key to check current variables"; 
	while (1) {
		_getch();
		_getch();
		cout << endl;
		for (int i = 0; i < n; ++i)
			cout << strings[i].c_str() << endl;
	}

}