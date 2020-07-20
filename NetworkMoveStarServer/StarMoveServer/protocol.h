#pragma once

#define SERVERPORT 3000
#define SERVERIP L"0.0.0.0"

#define SCREEN_WIDTH 82
#define SCREEN_HEIGHT 24

enum MESSAGE { GETID = 0, CREATESTAR, DELETESTAR, STARMOVE };

struct IdHeader {
	int type;
	int starId;
	char temp[8];
};

struct CreateStar {
	int type;
	int starId;
	int X;
	int Y;
};

struct DeleteStar {
	int type;
	int starId;
	char temp[8];
};


struct MoveStar {
	int type;
	int starId;
	int X;
	int Y;
};

struct MsgPacket {
	int type;
	int starId;
	int X;
	int Y;
};