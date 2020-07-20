// NetworkDrawPen.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"
#include "NetworkDrawPen.h"
#include "CRingBuffer.h"

#define WM_NETWORK (WM_USER+1)

#define SERVERPORT 25000
#define SERVERIP "127.0.0.1"

#define MAX_LOADSTRING 100


static int startX;

static int startY;

static int endX;

static int endY;

struct stHeader
{
    unsigned short LEn;
};

#pragma pack(1)
struct stDrawPacket
{
    int iStartX;
    int iStartY;
    int iEndX;
    int iEndY;
};
#pragma pack()

SOCKET clientSock;

bool bConnect = false;

bool sendFlag = false;

CRingBuffer rCq(2000);

CRingBuffer sCq(2000);


void ReadFunc();
void SendFunc();

void WriteBuffer(stHeader headerPacket,stDrawPacket drawPacket,int iSize);

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND hWnd;


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NETWORKDRAWPEN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NETWORKDRAWPEN));

    MSG msg;

    AllocConsole();
    freopen("CONOUT$", "r+t", stdout);
    //CONOUT$ - console 창
    //wt - 텍스트 쓰기 모드
    //stdout - 출력될 파일 포인터(모니터로 지정)


    int retval;

    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf_s("WSAStart up error : %d", WSAGetLastError());
        return -1;
    }

    clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock == INVALID_SOCKET)
    {
        printf_s("socket error : %d", WSAGetLastError());
        return -1;
    }


    linger lingerOpt;
    lingerOpt.l_onoff = 1;
    lingerOpt.l_linger = 0;
    retval = setsockopt(clientSock, SOL_SOCKET, SO_LINGER, (char*)&lingerOpt, sizeof(lingerOpt));
    if (retval == SOCKET_ERROR)
    {
        printf_s("setsockopt error : %d", WSAGetLastError());
        return -1;
    }

    WSAAsyncSelect(clientSock, hWnd, WM_NETWORK, FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT);

    
    SOCKADDR_IN serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVERPORT);
    InetPtonA(AF_INET, SERVERIP, &serverAddr.sin_addr);


    retval = connect(clientSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    if (retval == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            printf_s("connect error : %d", WSAGetLastError());
            return -1;
        }
    }

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NETWORKDRAWPEN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    static bool drawCheck = false;


    switch (message)
    {
    case WM_NETWORK:

        if (WSAGETSELECTERROR(lParam)) 
        {
            printf_s("connect error\n");
            closesocket(clientSock);
            return 0;
        }

        switch (WSAGETSELECTEVENT(lParam))
        {
        case FD_CONNECT:
            bConnect = true;
            printf_s("connect success\n");

            break;
        case FD_WRITE:
            
            sendFlag = true;
            SendFunc();

            break;
        case FD_READ:
            printf_s("read\n");
            ReadFunc();

            break;
        case FD_CLOSE:
            printf_s("FD_CLOSE\n");
            break;
        }


        break;
    case WM_LBUTTONDOWN:

        if (bConnect)
        {
            drawCheck = true;
        }

        
        endX = LOWORD(lParam);

        endY = HIWORD(lParam);
        
        
        break;

    case WM_LBUTTONUP:

        drawCheck = false;

        break;

    case WM_MOUSEMOVE:
        
        if (drawCheck)
        {
            startX = endX;
            startY = endY;


            endX = LOWORD(lParam);

            endY = HIWORD(lParam);

        
            stHeader header = {16};
            stDrawPacket drawPacket;
            drawPacket.iStartX = startX;
            drawPacket.iStartY = startY;
            drawPacket.iEndX = endX;
            drawPacket.iEndY = endY;

            WriteBuffer(header, drawPacket, 16);

            //startX = endX;
            //startY = endY;

        } 
        break;
   
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void ReadFunc()
{
    int retval;

    char buffer[1000];

    HDC hDc = GetDC(hWnd);

    retval = recv(clientSock, buffer, 1000, 0);
    if (retval == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            printf_s("recv 연결 끊기");
            closesocket(clientSock);
            return;
        }
        return;
    }

    int EnSize;

    EnSize = rCq.Enqueue(buffer, retval);
    if (EnSize != retval)
    {
        closesocket(clientSock);
        printf_s("enqueue 연결 끊기");
        return;
    }

    while (1)
    {
        if (rCq.GetUseSize() < 2)
        {
            return;
        }

        stHeader checkLen;

        int pRetval;
        pRetval = rCq.Peek((char*)&checkLen, 2);
        if (pRetval < 2)
        {
            printf_s("peek check");
            return;
        }

        if (checkLen.LEn + 2 > rCq.GetUseSize())
        {
            return;
        }

        rCq.MoveFront(2);

        int DeSize;

        stDrawPacket drawPacket;

        DeSize = rCq.Dequeue((char*)&drawPacket, checkLen.LEn);
        if (DeSize != checkLen.LEn)
        {
            closesocket(clientSock);
            printf_s("Dequeue error\n");
            return;
        }

        int sX;
        int sY;
        int eX;
        int eY;

        sX = drawPacket.iStartX;
        sY = drawPacket.iStartY;
        eX = drawPacket.iEndX;
        eY = drawPacket.iEndY;


        
        MoveToEx(hDc, sX, sY, NULL);
        LineTo(hDc, eX, eY);


    }
    ReleaseDC(hWnd, hDc);

}

void WriteBuffer(stHeader headerPacket, stDrawPacket drawPacket, int iSize)
{ 
    int retval;

    retval = sCq.Enqueue((char*)&headerPacket, 2);
    if (retval != 2)
    {
        printf_s("header enqueue error\n");
        closesocket(clientSock);
        return;
    }

    retval = sCq.Enqueue((char*)&drawPacket, iSize);
    if (retval != iSize)
    {
        printf_s("msg enqueue error\n");
        closesocket(clientSock);
        return;
    }

    if(sendFlag)
    { 
        SendFunc();
    } 
}

void SendFunc()
{
    while (1)
    {
        if (sCq.GetUseSize() == 0)
        {
            return;
        }

        char buffer[1000];

        int bRet;
        bRet = sCq.Peek(buffer, 1000);
        if (bRet == 0)
        {
            printf_s("sCq peek error\n");
            closesocket(clientSock);
            return;
        }
        
        int retval;

        retval = send(clientSock, buffer, bRet, 0);
        if (retval == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
                printf_s("send error\n");
                closesocket(clientSock);
                return;
            }
            sendFlag = false;
        }

        sCq.MoveFront(retval);
    }
}