﻿// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#pragma comment(lib,"Ws2_32")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>



#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include <iostream>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>