#include "stdafx.h"
#include "Server.h"

int main()
{
	srand((unsigned int)time(NULL));

	CServer Server{};

	Server.InitServer();
	Server.ProcessGameData();	
}
