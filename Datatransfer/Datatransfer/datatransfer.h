/*************************************************************************\
*    Datatransfer - libTISCH demo - see http://tisch.sourceforge.net/     *
*     Copyright (c) 2012 by Norbert Wiedermann, <wiederma@in.tum.de>      *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/
#pragma once
#ifdef _MSC_VER
	#define _AFXDLL
	#include <Afxwin.h>
	#include <WinSock2.h>
	//#include <WinSock.h>
	typedef int socklen_t;
	
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#define	INVALID_SOCKET -1
	#define	SOCKET_ERROR -1
	#define SOCKET int
	#define SOCKADDR struct sockaddr
#endif

#include <map>
static const struct sockaddr_in zero_sockaddr_in;
void activateMarker(int markerID);

class TcpRequestThread {
public:
	int markerID;
	SOCKET socket;
	sockaddr_in from;
	
	static UINT TcpRequestThreadStaticEntryPoint(LPVOID pThis) {
		TcpRequestThread* pthisTcpRequest = (TcpRequestThread*) pThis;
		pthisTcpRequest->TcpRequestThreadEntryPoint();
		delete pThis;
		return 1;
	}

	void setSocket(SOCKET mySocket, sockaddr_in from);
	
	//void activateMarker(int markerID);

	void TcpRequestThreadEntryPoint();
};

class TcpServerThread {
public:
	SOCKET AcceptSocket;
	SOCKET ListenSocket;
	int listenPort;
	sockaddr_in local;
	sockaddr_in from;

	static  UINT TcpServerThreadStaticEntryPoint(LPVOID pThis) {
		TcpServerThread* pthisTcpServer = (TcpServerThread*)pThis;
		pthisTcpServer->listenPort = 8080;
		pthisTcpServer->TcpServerThreadEntryPoint();
		return 0;
	}

	void initNetwork();

	void TcpServerThreadEntryPoint();
};

class SendToMobile {
public:
	sockaddr_in serv_addr;
	SOCKET mobileSocket;
	int mobilePort;
	ULONG mobileIP;
	unsigned char* messageToMobile;
	int messageSize;

	static UINT SendToMobileStaticEntryPoint(LPVOID pThis) {
		SendToMobile* pthisSendToMobile = (SendToMobile*) pThis;
		pthisSendToMobile->SendToMobileEntryPoint();
		delete pThis;
		return 1;
	}

	void connectToMobile();

	void SendToMobileEntryPoint();

	void sendMessageToMobile(unsigned char* msg, int amountOfBytes);

};
