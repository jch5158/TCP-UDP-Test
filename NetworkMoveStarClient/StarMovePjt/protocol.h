#pragma once

#define SERVERPORT 3000

enum MESSAGE { GETID = 0, CREATESTAR, DELETESTAR, UPDATE };

struct idHeader {
	int type;
	int starId;
	char temp[8];
};

struct createStar {
	int type;
	int starId;
	int X;
	int Y;
};

struct deleteStar {
	int type;
	int starId;
	char temp[8];
};


struct updateStar {
	int type;
	int starId;
	int X;
	int Y;
};
