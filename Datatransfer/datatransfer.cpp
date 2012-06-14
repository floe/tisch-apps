/*************************************************************************\
*    Datatransfer - libTISCH demo - see http://tisch.sourceforge.net/     *
*     Copyright (c) 2012 by Norbert Wiedermann, <wiederma@in.tum.de>      *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/
#include "Datatransfer\datatransfer.h"

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

MarkerID* pMarkerID;

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

void TcpRequestThread::setSocket(int _markerID, SOCKET _socket, sockaddr_in _from) {

	markerID = _markerID;
	socket = _socket;
	from = _from;
}

void TcpRequestThread::TcpRequestThreadEntryPoint() {
	cout << "handle request" << endl;
	// =========================================================
	// from client
	// =========================================================

	
	// =========================================================
	// to client
	// =========================================================
	
	char temp[512];
	sprintf(temp, "Your IP is %s\r\n",inet_ntoa(from.sin_addr));
	send(socket, temp, strlen(temp), 0);
	cout << "Connection from " << inet_ntoa(from.sin_addr) << endl;
	closesocket(socket);

	cout << "done" << endl;
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

	// data structure to manage open connections
	map<int, SOCKET> openConnections;
	map<int, SOCKET>::iterator it;
	pair<map<int, SOCKET>::iterator, bool> ret;

	cout << "waiting for connections" << endl;
	while(true) {
		
		AcceptSocket = accept(ListenSocket, (struct sockaddr*) &from, &fromlen);
		if (AcceptSocket == INVALID_SOCKET) {
			cout << endl << "accept() failed" << endl;
			closesocket(ListenSocket);
#ifdef _MSC_VER
			WSACleanup();
#endif	
			exit(5);
		}

		int useMarkerID = -1;

		for(int i = 0; i < 6; i++) {
			if(pMarkerID[i].thread == NULL) {
				// use this ID no thread attached
				useMarkerID = pMarkerID[i].markerID;

				break;
			}
			
		}

		if(useMarkerID == -1) {
			cout << "no marker ID left, try later again" << endl;
			closesocket(AcceptSocket);
		}
		else {
			pair<int, SOCKET> connection;
			connection.first = useMarkerID;
			connection.second = AcceptSocket;
			openConnections.insert(connection);

			cout << "used iD: " << useMarkerID << endl;

			TcpRequestThread* request = new TcpRequestThread();

			for(int i = 0; i < 6; i++) {
				if(useMarkerID == pMarkerID[i].markerID) {
					pMarkerID[i].thread = request;
					break;
				}
			
			}
		
			request->setSocket(connection.first, connection.second, from);
			AfxBeginThread(TcpRequestThread::TcpRequestThreadStaticEntryPoint, (void*)request);
		}
		//char temp[512];
		//sprintf(temp, "Your IP is %s\r\n",inet_ntoa(from.sin_addr));
		//send(AcceptSocket, temp, strlen(temp), 0);
		//cout << "Connection from " << inet_ntoa(from.sin_addr) << endl;
		//closesocket(AcceptSocket);

	} // while(true)

	closesocket(ListenSocket);
#ifdef _MSC_VER
	WSACleanup();
#endif
	exit(0);
}

int main( int argc, char* argv[] ) {
	cout << "Datatransfer - libTISCH demo" << endl;
	cout << "by Norbert Wiedermann (wiederma@in.tum.de) 2012" << endl;

	pMarkerID = new MarkerID[6];
	pMarkerID[0].markerID = 4648; pMarkerID[0].thread = NULL; // 1228
	pMarkerID[1].markerID = 7236; pMarkerID[1].thread = NULL; // 1c44
	pMarkerID[2].markerID = 2884; pMarkerID[2].thread = NULL; // 0b44
	pMarkerID[3].markerID = 1680; pMarkerID[3].thread = NULL; // 0690
	pMarkerID[4].markerID = 90;   pMarkerID[4].thread = NULL; // 005a
	pMarkerID[5].markerID = 626 ; pMarkerID[5].thread = NULL; // 0272
	
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
