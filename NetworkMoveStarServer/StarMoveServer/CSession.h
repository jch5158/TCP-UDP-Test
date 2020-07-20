#pragma once
class CSession
{
public:

	CSession(int x = 40, int y = 12);

	SOCKET mSocket;

	WCHAR userAddr[30];
	unsigned short userPort;

	DWORD mFlag;
	DWORD mID;
	DWORD mX;
	DWORD mY;

};

