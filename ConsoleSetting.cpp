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

void hideCursor() {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
}

// Hàm lấy vị trí hiện tại của con trỏ trên màn hình console
COORD getConsoleCursorPosition(HANDLE hConsoleOutput) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(hConsoleOutput, &csbi)) {
		return csbi.dwCursorPosition;
	}
	else {
		// Trả về tọa độ (-1, -1) nếu có lỗi
		return { -1, -1 };
	}
}