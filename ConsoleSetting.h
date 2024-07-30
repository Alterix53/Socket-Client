#pragma once
#include <Windows.h>

#define _AFXDLL
// https://codelearn.io/sharing/windowsh-ham-dinh-dang-noi-dung-console
// Go to the position X, Y on the console window
void GoToXY(int posX, int posY);
void hideCursor();
COORD getConsoleCursorPosition(HANDLE hConsoleOutput);