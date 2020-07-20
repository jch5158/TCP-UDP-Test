#include "stdafx.h"
#include "console.h"

char screenBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];


void cs_Initial(void) {
	CONSOLE_CURSOR_INFO stConsoleCursor; // 커서 구조체

	stConsoleCursor.bVisible = FALSE; // 커서 안보이게
	stConsoleCursor.dwSize = 1; // 커서의 최소 크기

	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &stConsoleCursor);

}



void MoveCursor(int iPosX, int iPosY) {
	COORD pos;
	pos.X = iPosX;
	pos.Y = iPosY;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}



// 버퍼의 내용을 화면으로 찍어주는 함수.
void BufferFlip(char(*screenBuffer)[SCREEN_WIDTH])
{

	for (int iCnt = 0; iCnt < SCREEN_HEIGHT; iCnt++)
	{
		MoveCursor(0, iCnt + 1);
		printf_s(screenBuffer[iCnt]);
	}
}


// 화면 버퍼를 지워주는 함수
void BufferClear(char(*screenBuffer)[SCREEN_WIDTH])
{
	int iCnt;
	memset(screenBuffer, ' ', SCREEN_WIDTH * SCREEN_HEIGHT);

	for (iCnt = 0; iCnt < SCREEN_HEIGHT; iCnt++)
	{
		screenBuffer[iCnt][SCREEN_WIDTH - 1] = '\0';
	}
}


// 버퍼의 특정 위치에 원하는 문자를 출력.
void SpriteDrawChar(char(*screenBuffer)[SCREEN_WIDTH], int iX, int iY, char chSprite)
{
	if (iX < 0 || iY < 0 || iX >= SCREEN_WIDTH - 1 || iY >= SCREEN_HEIGHT)
		return;
	screenBuffer[iY][iX] = chSprite;
}

void SpriteDrawStr(char(*screenBuffer)[SCREEN_WIDTH], int iX, int iY, const char* strSprite)
{
	if (iX < 0 || iY < 0 || iX + strlen(strSprite) >= SCREEN_WIDTH - 1 || iY >= SCREEN_HEIGHT)
		return;

	for (int strCnt = 0; strCnt < strlen(strSprite); ++strCnt)
	{
		screenBuffer[iY][iX + strCnt] = strSprite[strCnt];
	}
}

// 스크린 버퍼 가장자리를 원하는 문자로 출력
void SpriteDrawEdge(char(*screenBuffer)[SCREEN_WIDTH], char chSprite)
{
	for (int wid = 0; wid < SCREEN_WIDTH - 1; ++wid)
	{
		screenBuffer[0][wid] = chSprite;
		screenBuffer[SCREEN_HEIGHT - 1][wid] = chSprite;
	}

	for (int hei = 0; hei < SCREEN_HEIGHT; ++hei)
	{
		screenBuffer[hei][0] = chSprite;
		screenBuffer[hei][SCREEN_WIDTH - 2] = chSprite;
	}
}


void cs_ClearScreen(void) { // 버퍼 밀기
	unsigned long dw;
	FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', 100 * 100, { 0,0 }, &dw);
}
