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

class TcpRequestThread {
public:
	int markerID;
	SOCKET socket;
	sockaddr_in from;
	int protocolStep;

	static UINT TcpRequestThreadStaticEntryPoint(LPVOID pThis) {
		TcpRequestThread* pthisTcpRequest = (TcpRequestThread*) pThis;
		pthisTcpRequest->TcpRequestThreadEntryPoint();
		return 1;
	}

	void setSocket(int markerID, SOCKET mySocket, sockaddr_in from);

	void TcpRequestThreadEntryPoint();

	void activateMarker();
};

class TcpServerThread {
public:
	SOCKET AcceptSocket;
	SOCKET ListenSocket;
	int PORT;
	sockaddr_in local;
	sockaddr_in from;

	static  UINT TcpServerThreadStaticEntryPoint(LPVOID pThis) {
		TcpServerThread* pthisTcpServer = (TcpServerThread*)pThis;
		pthisTcpServer->PORT = 8080;
		pthisTcpServer->TcpServerThreadEntryPoint();
		return 0;
	}

	void initNetwork();

	void TcpServerThreadEntryPoint();
};

struct MarkerID {
	int markerID;
	TcpRequestThread* thread;

};
