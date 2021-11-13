#include "stdafx.h"
#include "Server.h"

int main()
{
	CServer Server{};

	Server.InitServer();
	Server.AcceptClient();
}