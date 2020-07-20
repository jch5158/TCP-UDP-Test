#pragma once

#define SCREEN_WIDTH 82
#define SCREEN_HEIGHT 24


extern char screenBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];


// X, Y 좌표로 커서 이동
void MoveCursor(int iX, int iY);

// 버퍼의 내용을 화면으로 찍어주는 함수.
void BufferFlip(char(*screenBuffer)[SCREEN_WIDTH]);

// 화면 버퍼를 지워주는 함수
void BufferClear(char(*screenBuffer)[SCREEN_WIDTH]);

// 버퍼의 특정 위치에 원하는 문자를 출력.
void SpriteDrawChar(char(*screenBuffer)[SCREEN_WIDTH], int iX, int iY, char chSprite);

// 버퍼의 특정 위치에 원하는 문자열을 출력.
void SpriteDrawStr(char(*screenBuffer)[SCREEN_WIDTH], int iX, int iY, const char* strSprite);

// 스크린 버퍼 가장자리 원하는 문자로 출력
void SpriteDrawEdge(char(*screenBuffer)[SCREEN_WIDTH], char strSprite);

void cs_ClearScreen(void);

void cs_Initial(void);