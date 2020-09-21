#include "CRS232.h"



CRS232::CRS232(char * port, int bRate, int bSize, int parity, int stopByte, Parser * parser)
{
	// Open COM port :
	hCom = CreateFile(port, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
	
	// Configure link parameters :
	DCB conf;
	GetCommState(hCom, &conf);
	conf.BaudRate = bRate;
	conf.ByteSize = bSize;
	conf.Parity = parity;
	conf.StopBits = stopByte;
	SetCommState(hCom, &conf);

	// Set timeouts :
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	SetCommTimeouts(hCom, &timeouts);

	//Configure Windows to Monitor the serial device for Character Reception
	//SetCommMask(hCom, EV_RXCHAR);

	// Memorize parser :
	this->parser = parser;

	// Start data receiver thread :
	stopThread = false;
	startReceiveThread();
}



CRS232::~CRS232()
{
	CloseHandle(hCom);
}

void CRS232::startReceiveThread()
{
	DWORD threadId;
	threadHandle = CreateThread(NULL, NULL, CRS232::receiveThread, this, NULL, &threadId);
}

DWORD WINAPI CRS232::receiveThread(LPVOID params)
{
	CRS232 * comm = (CRS232 *)params;
	DWORD dwEventMask;

	if (comm != NULL)
	{
		while (!comm->stopThread)
		{
			//if (WaitCommEvent(comm->hCom, &dwEventMask, NULL) != FALSE)
				comm->receive();
		}
	}
	
	return 0;
}

void CRS232::receive()
{
	char buffer[1];
	DWORD nbBytes;

	if (ReadFile(hCom, buffer, 1, &nbBytes, NULL) == TRUE)
	{
		if(nbBytes > 0 && parser != NULL)
			parser->parse(buffer, nbBytes);
	}
}

void CRS232::sendBuf(char * buf, int len)
{
	DWORD sendBytes;
	//SetCommMask(hCom, EV_RXCHAR);
	WriteFile(hCom, buf, len, &sendBytes, NULL);
}