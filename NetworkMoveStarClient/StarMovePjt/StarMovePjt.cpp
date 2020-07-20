#include "stdafx.h"
#include "console.h"
#include "protocol.h"

#define MAX_CLIENT 1000

struct Client
{
	int clientId;
	int x;
	int y;

};


void InitClient();


void InputKeyboard();
void NetworkPro();
void Render();


int retval;

extern char screenBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

SOCKET gSock;
Client gClientList[MAX_CLIENT];
int gMyID = -1;

int gClientNum;
int gPacketNum;

bool gKeyCheck;

int main()
{

	//setlocale(LC_ALL, "Korean");

	InitClient();

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf_s("wsa start up error : %d", WSAGetLastError());
		return -1;
	}

	gSock  = socket(AF_INET, SOCK_STREAM, 0);

	LINGER optval;
	optval.l_onoff = 1;
	optval.l_linger = 0;
	retval = setsockopt(gSock, SOL_SOCKET, SO_LINGER, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR)
	{
		wprintf_s(L"setsock error : %d", WSAGetLastError());
		return -1;
	}


	char addrBuffer[100];

	printf_s("서버 IP를 입력해주세요 : ");

	fgets(addrBuffer, sizeof(addrBuffer), stdin);

	DWORD alen = strlen(addrBuffer);

	if (addrBuffer[alen - 1] == '\n')
	{
		addrBuffer[alen - 1] = '\0';
	}

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	InetPtonA(AF_INET, addrBuffer, &serveraddr.sin_addr);

	// 넌블럭킹
	u_long on = 1;
	retval = ioctlsocket(gSock, FIONBIO, &on);
	if (retval == SOCKET_ERROR)
	{
		printf_s("소켓 전환 에러 : %d\n", WSAGetLastError());
		return -1;
	}

	retval = connect(gSock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			printf_s("connect error \n");
		}
	}

	// connect() 비동기 확인
	FD_SET readSet, writeSet;
	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);
	FD_SET(gSock, &readSet);
	FD_SET(gSock, &writeSet);

	timeval time{ 5, 0 };
	retval = select(0, NULL, &writeSet, NULL, &time);
	if (retval == SOCKET_ERROR)
	{
		printf("Error select()\n");
	}

	if (retval > 0) {
		if (FD_ISSET(gSock, &writeSet))
		{
			printf("connect() success\n");
		}
	}
	if (retval == 0) {
		printf("connect() fail\n");
		return 1;
	}

	system("cls");

	void cs_Initial(void);
	
	while (1)
	{
		InputKeyboard();
		NetworkPro();
		Render();
		Sleep(10);
	}
}


void InitClient()
{
	for (int cnt = 0; cnt < MAX_CLIENT; ++cnt)
	{
		gClientList[cnt].clientId = -1;
		gClientList[cnt].x = 0;
		gClientList[cnt].y = 0;
	}
}


void InputKeyboard() {

	if (gMyID == -1)
	{
		gKeyCheck = false;
		return;
	}


	gKeyCheck = true;

	if (GetAsyncKeyState(VK_LEFT) && GetAsyncKeyState(VK_UP))
	{
		if (gClientList[gMyID].x > 0)
		{
			gClientList[gMyID].x--;
		}

		if (gClientList[gMyID].y > 0)
		{
			gClientList[gMyID].y--;
		}

		return;
	}
	else if (GetAsyncKeyState(VK_RIGHT) && GetAsyncKeyState(VK_UP))
	{
		if (gClientList[gMyID].x < SCREEN_WIDTH-2)
		{
			gClientList[gMyID].x++;
		}

		if (gClientList[gMyID].y > 0)
		{
			gClientList[gMyID].y--;
		}

		return;
	}
	else if(GetAsyncKeyState(VK_LEFT) && GetAsyncKeyState(VK_DOWN))
	{
	
		if (gClientList[gMyID].x > 0)
		{
			gClientList[gMyID].x--;
		}

		if (gClientList[gMyID].y < SCREEN_HEIGHT-1)
		{
			gClientList[gMyID].y++;
		}
		return;
	}
	else if (GetAsyncKeyState(VK_RIGHT) && GetAsyncKeyState(VK_DOWN))
	{

		if (gClientList[gMyID].x < SCREEN_WIDTH-2)
		{
			gClientList[gMyID].x++;
		}

		if (gClientList[gMyID].y < SCREEN_HEIGHT-1)
		{
			gClientList[gMyID].y++;
		}
		return;
	}
	else if (GetAsyncKeyState(VK_UP))
	{
		if (gClientList[gMyID].y > 0)
		{
			gClientList[gMyID].y--;
		}
		return;
	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
		if (gClientList[gMyID].y < SCREEN_HEIGHT-1)
		{
			gClientList[gMyID].y++;
		}
		return;
	}
	
	else if (GetAsyncKeyState(VK_LEFT)) 
	{
		if (gClientList[gMyID].x > 0)
		{
			gClientList[gMyID].x--;
		}
		return;
	}
	
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		if (gClientList[gMyID].x < SCREEN_WIDTH-2)
		{
			gClientList[gMyID].x++;
		}

		return;
	}
	
	

	gKeyCheck = false;

	return;
}


void NetworkPro()
{
	FD_SET readSet;
	int retval;

	gPacketNum = 0;

	//////////////////////////////
	if (gKeyCheck)
	{
		updateStar movePacket;
		movePacket.type = UPDATE;
		movePacket.starId = gMyID;
		movePacket.X = gClientList[gMyID].x;
		movePacket.Y = gClientList[gMyID].y;

		int err = send(gSock, (char*)&movePacket, sizeof(movePacket), 0);
		if (err == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				printf_s("Error send()\n");
			}
		}
	}

	///////////////////////////////

	while (1)
	{
		FD_ZERO(&readSet);
		FD_SET(gSock, &readSet);

		// timeout은 10ms
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 10000;

		retval = select(0, &readSet, NULL, NULL, &timeout);
		if (retval == SOCKET_ERROR)
		{
			printf_s("Error select()\n");
		}

		if (retval > 0)
		{
			updateStar packet;
			if (FD_ISSET(gSock, &readSet))
			{
				int err = recv(gSock, (char*)&packet, 16, 0);
				if (err == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						printf_s("Error recv()\n");
					}
				}

				switch (packet.type)
				{
				case GETID:
				{
					gMyID = packet.starId;
					gPacketNum++;
				}
				break;
				case CREATESTAR:
				{
					if (gClientList[packet.starId].clientId == -1)
					{
						gClientList[packet.starId].clientId = packet.starId;
						gClientList[packet.starId].x = packet.X;
						gClientList[packet.starId].y = packet.Y;
						gClientNum++;
					}
					else
					{
						printf_s("Error CreateStar Packet\n");
					}
					gPacketNum++;
				}
				break;
				case DELETESTAR:
				{
					if (gClientList[packet.starId].clientId != -1)
					{
						gClientList[packet.starId].clientId = -1;
						gClientList[packet.starId].x = 0;
						gClientList[packet.starId].y = 0;
						gClientNum--;
					}
					else
					{
						printf_s("Error Delete Star Packet\n");
					}
					gPacketNum++;
				}
				break;
				case UPDATE:
				{
					if (gClientList[packet.starId].clientId != -1)
					{
						gClientList[packet.starId].x = packet.X;
						gClientList[packet.starId].y = packet.Y;
					}
					else
					{
						printf_s("Error MoveStar Packet\n");
					}
					gPacketNum++;
				}
				break;
				}
			}
		}
		if (retval == 0)
		{
			break;
		}
	}

}

void Render()
{
	BufferClear(screenBuffer);


	for (int cnt = 0; cnt < MAX_CLIENT; ++cnt)
	{
		if (gClientList[cnt].clientId == -1)
		{
			continue;
		}

		SpriteDrawChar(screenBuffer, gClientList[cnt].x, gClientList[cnt].y, '*');
	}

	MoveCursor(0, 0);

	printf("Client : %d / Pakcet : %d\n", gClientNum, gPacketNum);


	BufferFlip(screenBuffer);

}
