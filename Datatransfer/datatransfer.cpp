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
	#include <WinSock.h>
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
#include <sstream>
#include "BlobMarker.h"
#include "BlobCount.h"
#include "BlobPos.h"

using namespace std;

Window* win = 0;

class HandyDropZone: public Tile {
public: HandyDropZone( int _x, int _y, RGBATexture* _tex):
	Tile(100,200, _x, _y, 0, _tex, TISCH_TILE_MOVE|TISCH_TILE_SLIDE|TISCH_TILE_BOUNCE )
	{
		shadow = true;
	}
};

class Handy: public Container {
public:
	Handy(int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0xFF):
	Container( _w, _h, _x, _y, angle, _tex, mode)
	{
	  
		for(vector<Gesture>::iterator iter = region.gestures.begin();
			iter != region.gestures.end(); iter++)
		{
			if( iter->name() == "handy" ) {
				region.gestures.erase(iter);
				break;
			}

		}

		Gesture handy( "handy", GESTURE_FLAGS_DEFAULT|GESTURE_FLAGS_STICKY);

		//
		handy.push_back(new BlobMarker(1<<INPUT_TYPE_OBJECT));
		
		//
		BlobCount* bcnt = new BlobCount(1<<INPUT_TYPE_OBJECT);
		bcnt->bounds().push_back(0);
		bcnt->bounds().push_back(1);
		handy.push_back( bcnt );

		//
		handy.push_back(new BlobPos(1<<INPUT_TYPE_OBJECT));

		//
		region.gestures.push_back( handy );
	}

	void action( Gesture* gesture ) {
		if( gesture->name() == "handy" ) {

			FeatureBase* f = (*gesture)[1];
			BlobCount* p = dynamic_cast<BlobCount*>(f);

			if(p->result() == 1) {
				f = (*gesture)[0];
				BlobMarker* bm = dynamic_cast<BlobMarker*>(f);
				
				if(bm->markerID > 0) {
					RGBATexture* texture = new RGBATexture( TISCH_PREFIX "Box.png" );
					
					f = (*gesture)[2];
					BlobPos* bp = dynamic_cast<BlobPos*>(f);
					Vector blobPos = bp->result();
					transform(blobPos,1);
					blobPos.z = 0;
					blobPos.normalize();
					
					HandyDropZone* hdz = new HandyDropZone(blobPos.x, blobPos.z, texture);
					win->add(hdz);
				}

			} // if(p->result() == 1) {

		} // if( gesture->name() == "handy" ) {
		Container::action(gesture);

	} // void action( Gesture* gesture ) {
};

class TcpRequestThread {
public:
	static UINT TcpRequestThreadStaticEntryPoint(LPVOID pThis) {
		TcpRequestThread* pthisTcpRequest = (TcpRequestThread*) pThis;
		pthisTcpRequest->TcpRequestThreadEntryPoint();
		return 1;
	}

	void TcpRequestThreadEntryPoint() {
		cout << "handle request" << endl;
		// =========================================================
		// from client
		// =========================================================

		// =========================================================
		// to client
		// =========================================================
		cout << "done" << endl;
	}
};

class TcpServerThread {
public:
	SOCKET AcceptSocket;
	SOCKET ListenSocket;
	int PORT;
	sockaddr_in local;
	sockaddr_in from;

	static UINT TcpServerThreadStaticEntryPoint(LPVOID pThis) {
		TcpServerThread* pthisTcpServer = (TcpServerThread*)pThis;
		pthisTcpServer->PORT = 8080;
		pthisTcpServer->TcpServerThreadEntryPoint();
		return 0;
	}

	void TcpServerThread::initNetwork() {
		cout << "starting up TCP server ... ";

		WSADATA wsaData;
	
		int wsaret = WSAStartup(MAKEWORD(2,2), &wsaData);
		if(wsaret != 0) {
			cout << endl << "WSAStartup failed" << endl;
			exit(1);
		}

		local.sin_family = AF_INET;
		local.sin_addr.s_addr = INADDR_ANY;
		local.sin_port = htons(PORT);

		ListenSocket = socket(AF_INET, SOCK_STREAM, 0);

		if(ListenSocket == INVALID_SOCKET) {
			cout << endl << "socket() failed" << endl;
	#ifdef _MSC_VER
			WSACleanup();
	#endif
			exit(2);
		}

		if(bind(ListenSocket, (sockaddr*)&local, sizeof(local)) != 0) {
			cout << endl << "bind() failed" << endl;
#ifdef _MSC_VER
		WSACleanup();
#endif
			exit(3);
		}

		if(listen(ListenSocket, 10) != 0) {
			cout << endl << "listen() failed" << endl;
	#ifdef _MSC_VER
			WSACleanup();
	#endif
			exit(4);
		}
	}

	void TcpServerThread::TcpServerThreadEntryPoint() {

		initNetwork();

		int fromlen = sizeof(from);

		cout << "waiting for connections" << endl;
		while(true) {
			char temp[512];
			AcceptSocket = accept(ListenSocket, (struct sockaddr*) &from, &fromlen);
			if (AcceptSocket == INVALID_SOCKET) {
				cout << endl << "accept() failed" << endl;
				closesocket(ListenSocket);
	#ifdef _MSC_VER
				WSACleanup();
	#endif
				exit(5);
			}

			TcpRequestThread* request = new TcpRequestThread();
			AfxBeginThread(TcpRequestThread::TcpRequestThreadStaticEntryPoint, (void*)request);

			sprintf(temp, "Your IP is %s\r\n",inet_ntoa(from.sin_addr));
			send(AcceptSocket, temp, strlen(temp), 0);
			cout << "Connection from " << inet_ntoa(from.sin_addr) << endl;
			closesocket(AcceptSocket);
		}

		closesocket(ListenSocket);
	#ifdef _MSC_VER
		WSACleanup();
	#endif
		exit(0);
	}

};

int main( int argc, char* argv[] ) {
	cout << "Datatransfer - libTISCH demo" << endl;
	cout << "by Norbert Wiedermann (wiederma@in.tum.de) 2012" << endl;

	int width = 640;
	int height = 480;

	int mouse = ((argc > 1) && (std::string("-m") == argv[1]));
	win = new Window( width, height, "Datatransfer", mouse );
	win->texture(0);

	Handy* handy = new Handy( width, height );
	win->add(handy);

	win->update();
	
	// start TCP Server waiting for connections
	TcpServerThread* server = new TcpServerThread();
	AfxBeginThread(TcpServerThread::TcpServerThreadStaticEntryPoint, (void*)server);
	
	// keep looping on window thread
	win->run();

}
