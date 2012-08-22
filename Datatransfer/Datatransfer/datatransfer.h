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

#include <stdlib.h>
#include <nanolibc.h>
#include "Window.h"
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <sstream>
#include "BlobMarker.h"
#include "BlobCount.h"
#include "BlobPos.h"

#include "Label.h"
#include "Motion.h"
#include <map>

using namespace std;

static const struct sockaddr_in zero_sockaddr_in;
void activateMarker(int markerID);

class HandyDropZone : public Container {
public:
	HandyDropZone( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0x05):
	Container( _w, _h, _x, _y, 0, _tex, mode ) {
		shadow = true;
	};

	virtual void deleteMe() {
		cout << "HDZ deleteMe()" << endl;
		parent->remove(this);
		delete this;
	}
};

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

class MyImage : public Container {
public: MyImage(int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0xFF):
    Container( _w, _h, _x, _y, angle, _tex, mode) {
	};

	virtual void setImage(char* imgName);
	virtual void action( Gesture* gesture);
	virtual void draw();
	virtual void deleteMe() {
		cout << "MyImage deleteMe()" << endl;
		parent->remove(this);
		delete this;
	}

protected:
	char* imageName;
};

class InteractionArea : public Container {
public:
	InteractionArea(int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0x00):
	Container( _w, _h, _x, _y, angle, _tex, mode)
	{
		Gesture handy( "handy", GESTURE_FLAGS_DEFAULT|GESTURE_FLAGS_STICKY);
		handy.push_back(new BlobMarker(1<<INPUT_TYPE_FINGER));
		handy.push_back(new BlobPos(1<<INPUT_TYPE_FINGER));
		region.gestures.push_back( handy );
	}

	virtual void action( Gesture* gesture );
};

struct MarkerID {
	int markerID;
	TcpRequestThread* thread;
	sockaddr_in connectInfoMobile;
	bool active;
	HandyDropZone* hdz;
	std::vector<MyImage*> imageVector;
};