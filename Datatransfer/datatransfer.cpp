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

#include "Label.h"
#include "Motion.h"

using namespace std;

Window* win = 0;

MarkerID* pMarkerID;

class HandyDropZone: public Container {
public:
	HandyDropZone( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0xFF):
	Container( _w, _h, _x, _y, 0, _tex, mode )
	{
		shadow = true;
	}
};

class Handy: public Container {
public:
	Handy(int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0x00):
	Container( _w, _h, _x, _y, angle, _tex, mode)
	{
	  
		for(vector<Gesture>::iterator iter = region.gestures.begin();
			iter != region.gestures.end(); iter++)
		{
			if( iter->name() == "move" ) {
				region.gestures.erase(iter);
				break;
			}

		}

		Gesture handy( "handy", GESTURE_FLAGS_DEFAULT|GESTURE_FLAGS_STICKY);

		//
		handy.push_back(new BlobMarker(1<<INPUT_TYPE_FINGER));
		
		//
		BlobCount* bcnt = new BlobCount(1<<INPUT_TYPE_FINGER);
		bcnt->bounds().push_back(0);
		bcnt->bounds().push_back(100);
		handy.push_back( bcnt );

		//
		handy.push_back(new BlobPos(1<<INPUT_TYPE_FINGER));

		//
		region.gestures.push_back( handy );
	}

	void action( Gesture* gesture ) {
		
		if( gesture->name() == "handy" ) {
			cout << "gesture: " << gesture->name() << endl;
			/*FeatureBase* f = (*gesture)[1];
			BlobCount* p = dynamic_cast<BlobCount*>(f);
			cout << "result: " << p->result() << endl;
			if(p->result() == 1) {
			*/	
				FeatureBase* f = (*gesture)[0];
				BlobMarker* bm = dynamic_cast<BlobMarker*>(f);
				int markerIDtmp = bm->result();
				cout << "MID: " << markerIDtmp << endl;
				if(markerIDtmp > 0) {
					
					for(int i = 0; i < 6; i++) {
						if(markerIDtmp == pMarkerID[i].markerID) {
							cout << "activate marker" << endl;
							pMarkerID[i].thread->activateMarker();
							break;
						}
			
					}

					RGBATexture* texture = new RGBATexture( TISCH_PREFIX "Box.png" );
					
					f = (*gesture)[2];
					BlobPos* bp = dynamic_cast<BlobPos*>(f);
					Vector blobPos = bp->result();
					transform(blobPos,1);
					blobPos.z = 0;
					blobPos.normalize();
					
					HandyDropZone* hdz;
					//HandyDropZone* hdz = new HandyDropZone(150, 200, blobPos.x, blobPos.y, 0.0, texture);
					
					switch(markerIDtmp) {
					case 4648:
						hdz = new HandyDropZone(50, 100, -250, 100, 0.0, texture);
						hdz->add( new Label("DropZone", 200, 20, 0, 50, 0, 1) );
						hdz->add( new Label("0x1228", 200, 20, 0, 30, 0, 1) );
						break;
					case 7236:
						hdz = new HandyDropZone(50, 100, 0, -10, 0.0, texture);
						hdz->add( new Label("DropZone", 200, 20, 0, 50, 0, 1) );
						hdz->add( new Label("0x1c44", 200, 20, 0, 30, 0, 1) );
						break;
					case 2884:
						hdz = new HandyDropZone(50, 100, 200, -80, 0.0, texture);
						hdz->add( new Label("DropZone", 200, 20, 0, 50, 0, 1) );
						hdz->add( new Label("0x0b44", 200, 20, 0, 30, 0, 1) );
						break;
					case 1680:
						hdz = new HandyDropZone(50, 100, 200, -80, 0.0, texture);
						hdz->add( new Label("DropZone", 200, 20, 0, 50, 0, 1) );
						hdz->add( new Label("0x0690", 200, 20, 0, 30, 0, 1) );
						break;
					case 90:
						hdz = new HandyDropZone(50, 100, 200, -80, 0.0, texture);
						hdz->add( new Label("DropZone", 200, 20, 0, 50, 0, 1) );
						hdz->add( new Label("0x005a", 200, 20, 0, 30, 0, 1) );
						break;
					case 626:
						hdz = new HandyDropZone(50, 100, 200, -80, 0.0, texture);
						hdz->add( new Label("DropZone", 200, 20, 0, 50, 0, 1) );
						hdz->add( new Label("0x0272", 200, 20, 0, 30, 0, 1) );
						break;
					}
					
					win->add(hdz);
				}

			//} // if(p->result() == 1) {

		} // if( gesture->name() == "handy" ) {
		Container::action(gesture);

	} // void action( Gesture* gesture ) {
};

void TcpRequestThread::setSocket(int _markerID, SOCKET _socket, sockaddr_in _from) {

	markerID = _markerID;
	socket = _socket;
	from = _from;
	protocolStep = 0;
}

void TcpRequestThread::TcpRequestThreadEntryPoint() {
	cout << "handle request" << endl;
	while(protocolStep < 2) {
		// =========================================================
		// from client
		// =========================================================
		int in_len;
		int recvBytes = 0;
		char inBuffer[4];
		//===================================================================
		// read header as int, how many chars will come
		recvBytes = recv(socket, inBuffer, sizeof inBuffer, MSG_WAITALL);
	
		// if more or less than 4 chars are received -> error was no int
		if(recvBytes != 4) {
			printf("Error: recv returned: %d\n", recvBytes);
		}
		else {
			printf("%d %d %d %d\n", inBuffer[0], inBuffer[1], inBuffer[2], inBuffer[3]);
		}
	
		memcpy(&in_len, &inBuffer, sizeof(int));
		in_len = ntohl(in_len);
		cout << "recvBytes: " << recvBytes << " inBuffer: " << in_len << endl;

		//===================================================================
		// read payload
		char* inMsgBuffer = new char[in_len];
		recvBytes = recv(socket, inMsgBuffer, in_len, MSG_WAITALL);
		if(recvBytes != in_len) {
			printf("Error: recv returned: %d\n", recvBytes);
		}
		else {
			for(int i = 0; i < in_len; i++) {
				printf("inMsgBuffer %d\n", inMsgBuffer[i]);
			}
		}

		//protocolStep = atoi(inMsgBuffer);
		//memcpy(&protocolStep, &inMsgBuffer, sizeof(char));
		cout << "protocolStep: " << protocolStep << endl;
		// compare inMsgBuffer to find protocol step

		// =========================================================
		// to client
		// =========================================================
		char out_len[4];
		char* toClient;
		int len, mID_endian;

		switch(protocolStep) {
		case 0: // requestID
			len = htonl(4);
			memcpy(&out_len, &len, 4);
			send(socket, (const char*) out_len, 4, 0);

			// send markerID
			//char toClient[4];
			toClient = new char[4];
			mID_endian = htonl(markerID);
			memcpy(toClient, &mID_endian, sizeof(int));
			send(socket, (const char*)toClient, 4, 0);
			protocolStep++;

			break;

		case 1: // waitForMarkerFound

			/*int random_integer;  
            random_integer = (rand()%40000)+10000;
			
			cout << "sleep" << endl;
			sleep(random_integer);
			*/
			cout << "wait for marker" << endl;

			break;	
		}

		// send length of message
		//send(socket, (const char*) out_len, 4, 0);
		// send payload
		//send(socket, (const char*) toClient, 4, 0);

		//char temp[512];
		//sprintf(temp, "Your IP is %s\r\n",inet_ntoa(from.sin_addr));
		//send(socket, temp, strlen(temp), 0);
		//cout << "Connection from " << inet_ntoa(from.sin_addr) << endl;
	}
	closesocket(socket);

	cout << "socket closed" << endl;
}

void TcpRequestThread::activateMarker() {
	char out_len[4];
	char* toClient;
	int len, dummy;
	len = htonl(4);
	memcpy(&out_len, &len, 4);
	send(socket, (const char*) out_len, 4, 0);

	// send marker found
	//char toClient[4];
	toClient = new char[4];
	dummy = htonl(1);
	memcpy(toClient, &dummy, sizeof(int));
	send(socket, (const char*)toClient, 4, 0);
	protocolStep++;
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

class MyContainer: public Container{
	public:

		MyContainer( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0xFF):
			Container( _w,_h,_x,_y,angle,_tex,mode) 
		{ 
			
			alpha = 1.0;
			for(std::vector<Gesture>::iterator it = region.gestures.begin(); it != region.gestures.end(); it++)
			{
				if(it->name() == "move")
				{
					region.gestures.erase(it);
					break;
				}
			}

			Gesture move( "move" , GESTURE_FLAGS_DEFAULT|GESTURE_FLAGS_STICKY);

			move.push_back(new Motion());
			BlobCount* bcnt = new BlobCount(1<<INPUT_TYPE_FINGER);
			bcnt->bounds().push_back( 0 );
			bcnt->bounds().push_back( 10000 );
			move.push_back( bcnt );
			region.gestures.push_back( move );	
		}

		void action( Gesture* gesture ) {
			if ( gesture->name() == "move" ) {
				
					FeatureBase* f = (*gesture)[1];
					BlobCount* p = dynamic_cast<BlobCount*>(f);
					if (p->result() == 3)
					{
						f = (*gesture)[0];
						Motion* m = dynamic_cast<Motion*>(f);
						Vector tmp = m->result();

						if(tmp.y < 0) alpha -= 0.01f;
						else alpha += 0.01f;
						if(alpha > 1.0) alpha = 1.0;
						else if(alpha < 0.2) alpha = 0.2;
						color(1,1,1,alpha);
						return;
					}
			}
			Container::action(gesture);
		}
		double alpha;
};

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
	handy->add( new Label("interaction area", 300, 20, 0, 120, 0, 1) );
	win->add(handy);

	win->update();
	
	// start TCP Server waiting for connections
	TcpServerThread* server = new TcpServerThread();
	AfxBeginThread(TcpServerThread::TcpServerThreadStaticEntryPoint, (void*)server);
	
	srandom(45890);
	// load example images as textures
	for (int i = mouse+1; i < argc; i++) {
		RGBATexture* tmp = new RGBATexture( argv[i] );
		MyContainer* img = new MyContainer( 
			tmp->width(1)/5, 
			tmp->height(1)/5,
			(int)(((double)random()/(double)RAND_MAX)*700-350),
			(int)(((double)random()/(double)RAND_MAX)*450-225),
			(int)(((double)random()/(double)RAND_MAX)*360),
			tmp, 0xFF
		);
		win->add( img );
	}

	win->update();
	// keep looping on window thread
	win->run();

}
