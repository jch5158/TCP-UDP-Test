#include "stdafx.h"
#include "CSession.h"



CSession::CSession(int x,int y):mX(x),mY(y)
{
	static DWORD setUserID;

	this->mID = setUserID;

	setUserID++;	
}