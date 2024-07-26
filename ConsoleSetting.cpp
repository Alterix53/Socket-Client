#include "ConsoleSetting.h"

// https://codelearn.io/sharing/windowsh-ham-dinh-dang-noi-dung-console
// Go to the position X, Y on the console window
void GoToXY(int posX, int posY)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD Position;
	Position.X = posX;
	Position.Y = posY;

	SetConsoleCursorPosition(hStdout, Position);
}