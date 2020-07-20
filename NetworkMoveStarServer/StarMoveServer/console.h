#pragma once

#define SCREEN_WIDTH 82
#define SCREEN_HEIGHT 24


extern char screenBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];


// X, Y ��ǥ�� Ŀ�� �̵�
void MoveCursor(int iX, int iY);

// ������ ������ ȭ������ ����ִ� �Լ�.
void BufferFlip(char(*screenBuffer)[SCREEN_WIDTH]);

// ȭ�� ���۸� �����ִ� �Լ�
void BufferClear(char(*screenBuffer)[SCREEN_WIDTH]);

// ������ Ư�� ��ġ�� ���ϴ� ���ڸ� ���.
void SpriteDrawChar(char(*screenBuffer)[SCREEN_WIDTH], int iX, int iY, char chSprite);

// ������ Ư�� ��ġ�� ���ϴ� ���ڿ��� ���.
void SpriteDrawStr(char(*screenBuffer)[SCREEN_WIDTH], int iX, int iY, const char* strSprite);

// ��ũ�� ���� �����ڸ� ���ϴ� ���ڷ� ���
void SpriteDrawEdge(char(*screenBuffer)[SCREEN_WIDTH], char strSprite);

void cs_ClearScreen(void);

void cs_Initial(void);