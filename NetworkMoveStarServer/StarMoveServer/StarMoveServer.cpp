#include "stdafx.h"
#include "console.h"
#include "protocol.h"
#include "CSession.h"
#include "CList.h"

CList<CSession*> sockList;

// 리슨 소켓
SOCKET gListenSocket;

FD_SET gRset;

int retval;

// 소켓 63개 되면은 안받기
DWORD dTotalSockets;


DWORD fpsPrint = 60;


bool renderCheck;


extern char screenBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

// 리슨 소켓 생성, 넌블럭킹 설정, 소켓 옵션 설정
bool setSocketOption();

// 네트워크 처리하는 함수이다.
void NetworkProc();

void Accept();
   
void Recv(CSession *session);

void Move(CSession* session,int x,int y);

void Disconnect(CSession* session);

void DeleteSession();

void Unicasting(CSession* session, MsgPacket& msgPacket, int packetSize);

void Broadcasting(CSession* session,MsgPacket& msgPacket,int packetSize);

void Render();

void FpsFunc();

int wmain()
{	
	timeBeginPeriod(1);

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
	{
		wprintf_s(L"WSAStartUp error : %d\n", WSAGetLastError());
		return -1;
	}	

	if (!setSocketOption())
	{
		return -1;
	}

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVERPORT);
	InetPtonW(AF_INET, SERVERIP, &serverAddr.sin_addr);

	retval = bind(gListenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR)
	{
		wprintf_s(L"bind error : %d\n", WSAGetLastError());
		return -1;
	}

	retval = listen(gListenSocket, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		wprintf_s(L"listen error : %d\n", WSAGetLastError());
		return -1;
	}


	cs_Initial();

	while (1)
	{
		renderCheck = false;

		NetworkProc();

		DeleteSession();
	

		if (renderCheck) 
		{
			Render();
		}
		
		//Sleep(16);
	}


	timeEndPeriod(1);
}

void NetworkProc()
{		

	FD_ZERO(&gRset);
	FD_SET(gListenSocket,&gRset);

	CList<CSession*>::Iterator iterE = sockList.end();

	for (CList<CSession*>::Iterator iter = sockList.begin(); iter != iterE; ++iter)
	{
		FD_SET(iter->mSocket,&gRset);
	}
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;

	retval = select(0, &gRset, NULL, NULL, &timeout);
	if (retval > 0)
	{
		if (FD_ISSET(gListenSocket, &gRset))
		{
			// 최대 받을 수 있는 소켓 리슨소켓 포함해서 64개 이다.
			if (dTotalSockets < 63)
			{
				Accept();			// 함수로 빼자
			}
		}		
		CList<CSession*>::Iterator iterE = sockList.end();

		for (CList<CSession*>::Iterator iter = sockList.begin(); iter != iterE; ++iter)
		{
			if (FD_ISSET(iter->mSocket,&gRset))
			{			
				Recv((*iter)->data);
			}
		}


	}

}

void Accept()
{
	SOCKET clientSock;
	SOCKADDR_IN clientAddr;
	int cAddrlen;

	MsgPacket msgPacket;
	CSession *session;

	cAddrlen = sizeof(clientAddr);
	
		clientSock = accept(gListenSocket, (SOCKADDR*)&clientAddr, &cAddrlen);
		if (clientSock == INVALID_SOCKET)
		{
			// 백로그에 뽑을게 없으면 wouldblock
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{				
				return;
			}
			wprintf_s(L"accept error : %d", WSAGetLastError());
			return;
		}
		else
		{
			dTotalSockets += 1;

			// 랜더할지 말지
			renderCheck = true;
			
			session = new CSession;
			session->mSocket = clientSock;
			session->userPort = ntohs(clientAddr.sin_port);
			InetNtopW(AF_INET, (void*)&clientAddr.sin_addr, session->userAddr, sizeof(session->userAddr));		
	
			sockList.PushBack(session);		

			//wprintf_s(L"connect ip : %s, port : %d\n", sockList.begin()->userAddr, sockList.begin()->userPort);

			msgPacket.starId = session->mID;
			msgPacket.type = MESSAGE::GETID;
			msgPacket.X = session->mX;
			msgPacket.Y = session->mY;

			Unicasting(session, msgPacket, 16);

			msgPacket.type = MESSAGE::CREATESTAR;

			Broadcasting(NULL, msgPacket, 16);


			CList<CSession*>::Iterator iterE = sockList.end();

			for (CList<CSession*>::Iterator iter = sockList.begin(); iter != iterE; ++iter)
			{
				if (session != (*iter)->data)
				{
					msgPacket.starId = iter->mID;
					msgPacket.type = MESSAGE::CREATESTAR;
					msgPacket.X = iter->mX;
					msgPacket.Y = iter->mY;
					Unicasting(session,msgPacket,16);
				}
			}

		}
}

void Recv(CSession* session)
{
	
	MsgPacket msgPacket;

	retval = recv(session->mSocket, (char*)&msgPacket, 16,0);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			Disconnect(session);
			return;
		}
		return;
	}	

	if (msgPacket.type != MESSAGE::STARMOVE)
	{
		Disconnect(session);
		return;
	}

	// 랜더할지 말지
	renderCheck = true;

	Move(session, msgPacket.X, msgPacket.Y);

	return;
}

void Disconnect(CSession* session)
{
	MsgPacket msgPacket;

	renderCheck = true;


	CList<CSession*>::Iterator iterE = sockList.end();

	for (CList<CSession*>::Iterator iter = sockList.begin(); iter != iterE; ++iter)
	{
		if ((*iter)->data == session)
		{
			(*iter)->deleteCheck = true;

			

			msgPacket.starId = session->mID;
			msgPacket.type = MESSAGE::DELETESTAR;
			
			Broadcasting(session, msgPacket, 16);

			return;
		}
	}
}

void Move(CSession* session, int x, int y)
{
	if (x > SCREEN_WIDTH - 2 || x < 0 || y < 0 || y > SCREEN_HEIGHT-1 )
	{
		return;
	}

	
	MsgPacket msgPacket;

	msgPacket.starId = session->mID;
	msgPacket.type = MESSAGE::STARMOVE;
	msgPacket.X = x;
	msgPacket.Y = y;

	session->mX = x;
	session->mY = y;

	Broadcasting(session, msgPacket, 16);

	return;
}

void Unicasting(CSession* session, MsgPacket& msgPacket, int packetSize)
{	
	retval = send(session->mSocket, (char*)&msgPacket, packetSize,0);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return;
		}
		else
		{
			Disconnect(session);
		}
	}	
	return;
}

void Broadcasting(CSession* session, MsgPacket& msgPacket, int packetSize)
{
	CList<CSession*>::Iterator iterE = sockList.end();
	
	for (CList<CSession*>::Iterator iter = sockList.begin(); iter != iterE; ++iter)
	{
		if (session != (*iter)->data)
		{

			retval = send(iter->mSocket, (char*)&msgPacket, packetSize, 0);
			if (retval == SOCKET_ERROR)
			{
				// 우드블럭이 아닐경우 디스커넥트
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					if ((*iter)->deleteCheck == false) 
					{
						Disconnect((*iter)->data);
					}
				}
			}
		}
	}

	return;
}

void DeleteSession()
{
	CList<CSession*>::Iterator iterE = sockList.end();

	for (CList<CSession*>::Iterator iter = sockList.begin(); iter != iterE;)
	{
		if ((*iter)->deleteCheck)
		{

			dTotalSockets -= 1;

			closesocket((*iter)->data->mSocket);

			delete (*iter)->data;

			iter = sockList.erase(iter);
		
			//wprintf_s(L"deleteNode\n");
		}
		else
		{
			++iter;
		}

	}

}

bool setSocketOption()
{
	int retval;

	gListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (gListenSocket == INVALID_SOCKET)
	{
		wprintf_s(L"socket create error : %d\n", WSAGetLastError());
		return false;
	}

	ULONG on = 1;
	retval = ioctlsocket(gListenSocket, FIONBIO, &on);
	if (retval == INVALID_SOCKET)
	{
		wprintf_s(L"ioctlsocket error : %d\n", WSAGetLastError());
		return false;
	}

	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 0;
	retval = setsockopt(gListenSocket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));
	if (retval == SOCKET_ERROR)
	{
		wprintf_s(L"linger error : %d\n", WSAGetLastError());
		return false;
	}


	return true;
}

void Render()
{
//	FpsFunc();

	BufferClear(screenBuffer);

	CList<CSession*>::Iterator iterE = sockList.end();

	for (CList<CSession*>::Iterator iter = sockList.begin(); iter != iterE; ++iter)
	{

		SpriteDrawChar(screenBuffer, iter->mX, iter->mY, '*');
	
	}


	MoveCursor(0, 0);

	printf("Connect Client : %d\n", dTotalSockets);

	BufferFlip(screenBuffer);


}

void FpsFunc()
{
	static clock_t timeCheck = timeGetTime();

	static DWORD fps;

	fps+=1;

	if (1000 <= timeGetTime() - timeCheck)
	{
		fpsPrint = fps;

		timeCheck = timeGetTime();

		fps = 0;
	}

}