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

#include "Window.h"
#include "Label.h"
#include "Motion.h"

using namespace std;

class HandyDropZone: public Container {
public:
	HandyDropZone( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0x05):
	Container( _w, _h, _x, _y, 0, _tex, mode )
	{
		shadow = true;
	}
};

struct MarkerID {
	int markerID;
	TcpRequestThread* thread;
	sockaddr_in connectInfoMobile;
	bool active;
	HandyDropZone* hdz;
};

Window* win = 0;

std::map<int, MarkerID> markers;

RGBATexture* textures[3];

class MyImage: public Container {
public:
    MyImage(int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0xFF):
    Container( _w, _h, _x, _y, angle, _tex, mode)
    {}

    void action( Gesture* gesture ) {
		if( gesture->name() == "release" )
		{
			for(map<int, MarkerID>::iterator it = markers.begin(); it != markers.end(); it++) {
				if(it->second.hdz == NULL)
					continue;
				if((x > it->second.hdz->x - ( 0.5f * it->second.hdz->w)) && (x < it->second.hdz->x + ( 0.5f * it->second.hdz->w)))
				{
					if((y > it->second.hdz->y - ( 0.5f * it->second.hdz->h)) && (y < it->second.hdz->y + ( 0.5f * it->second.hdz->h)))
					{
						//send "texture" to phone
						std::cout << "Copy me to " << it->first << std::endl;
						break;
					}
				}
			}
		}
        else
            Container::action(gesture);
    }
}; 

class Handy: public Container {
public:
	Handy(int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0x00):
	Container( _w, _h, _x, _y, angle, _tex, mode)
	{
		Gesture handy( "handy", GESTURE_FLAGS_DEFAULT|GESTURE_FLAGS_STICKY);
		handy.push_back(new BlobMarker(1<<INPUT_TYPE_FINGER));
		handy.push_back(new BlobPos(1<<INPUT_TYPE_FINGER));
		region.gestures.push_back( handy );
	}

	void action( Gesture* gesture ) {
		
		if( gesture->name() == "handy" ) {
			cout << "gesture: " << gesture->name() << endl;
			FeatureBase* f = (*gesture)[0];
			BlobMarker* bm = dynamic_cast<BlobMarker*>(f);
			int markerIDtmp = bm->result();
			cout << "MID: " << markerIDtmp << endl;
			if(markerIDtmp > 0 && !markers[markerIDtmp].active) {
					
				activateMarker(markerIDtmp);

				RGBATexture* texture = new RGBATexture( TISCH_PREFIX "Box.png" );
					
				f = (*gesture)[1];
				BlobPos* bp = dynamic_cast<BlobPos*>(f);
				Vector blobPos = bp->result();
				cout << "BlobPos: " << blobPos.x << " " << blobPos.y << endl;
				transform(blobPos,1);
				blobPos.z = 0;
				blobPos.normalize();
				cout << "BlobPos: " << blobPos.x << " " << blobPos.y << endl;
				markers[markerIDtmp].hdz = new HandyDropZone(150, 200, blobPos.x * win->getWidth() * 0.5f, blobPos.y * win->getHeight() * 0.5f, 0.0, new RGBATexture( "handy.png" ));
				markers[markerIDtmp].active = true;

				win->add(markers[markerIDtmp].hdz);
			}
		} // if( gesture->name() == "handy" )
	} // void action( Gesture* gesture )
};

void showImage(int markerID, int i)
{	
	MyImage* img = new MyImage( 
		200,//tmp->width(1)/5, 
		100, //tmp->height(1)/5,
		markers[markerID].hdz->x ,
		markers[markerID].hdz->y < 0 ? markers[markerID].hdz->y + markers[markerID].hdz->h : markers[markerID].hdz->y - markers[markerID].hdz->h,
		0,
		textures[i-1], 0x05
	);
	win->add( img );
}

void TcpRequestThread::setSocket(SOCKET _socket, sockaddr_in _from) {
	socket = _socket;
	from = _from;
}

void TcpRequestThread::TcpRequestThreadEntryPoint() {
	cout << "handle request" << endl;
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

	//Marker requested
	if( (int)inMsgBuffer[0] == 0 )
	{
		int useMarkerID = -1;

		for(map<int, MarkerID>::iterator it = markers.begin(); it != markers.end(); it++) {
			if(it->second.connectInfoMobile.sin_addr.S_un.S_addr == NULL) {
				useMarkerID = it->first;
				break;
			}
		}

		if(useMarkerID == -1) {
			cout << "no marker ID left, try later again" << endl;
			closesocket(socket);
		}

		markers[useMarkerID].connectInfoMobile = from;

		// =========================================================
		// to client
		// =========================================================
		// prepare header
		char out_len[4];
		int len, mID_endian;

		len = htonl(4); // send 4 char (1 int) network byte order
		memcpy(&out_len, &len, 4);
		send(socket, (const char*) out_len, 4, 0);

		// send markerID
		char toClient[4];
		mID_endian = htonl(useMarkerID); // change to network byte order
		memcpy(&toClient, &mID_endian, sizeof(int));
		send(socket, (const char*) toClient, 4, 0);
	}

	else
	{
		for(map<int, MarkerID>::iterator it = markers.begin(); it != markers.end(); it++) {
			if(it->second.connectInfoMobile.sin_addr.S_un.S_addr == from.sin_addr.S_un.S_addr) {
				showImage(it->first, (int)inMsgBuffer[0]);
				break;
			}
		}

		// =========================================================
		// to client
		// =========================================================
		// prepare header
		char out_len[4];
		int len, mID_endian;

		len = htonl(4); // send 4 char (1 int) network byte order
		memcpy(&out_len, &len, 4);
		send(socket, (const char*) out_len, 4, 0);

		// send markerID
		char toClient[4];
		mID_endian = htonl(0); // change to network byte order
		memcpy(&toClient, &mID_endian, sizeof(int));
		send(socket, (const char*) toClient, 4, 0);
	}

	//char temp[512];
	//sprintf(temp, "Your IP is %s\r\n",inet_ntoa(from.sin_addr));
	//send(socket, temp, strlen(temp), 0);
	//cout << "Connection from " << inet_ntoa(from.sin_addr) << endl;
	
	closesocket(socket);

	cout << "socket closed" << endl;
}

void SendToMobile::connectToMobile() {
	
	WSADATA wsaData;
	
	int wsaret = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(wsaret != 0) {
		cout << endl << "WSAStartup failed" << endl;
		exit(5);
	}

	mobileSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(mobileSocket == INVALID_SOCKET) {
		cout << endl << "socket() failed" << endl;
#ifdef _MSC_VER
		WSACleanup();
#endif
		exit(6);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = mobileIP;
	serv_addr.sin_port = htons(mobilePort);

	if(connect(mobileSocket, (sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		cout << endl << "sconnect() failed" << endl;
#ifdef _MSC_VER
		WSACleanup();
#endif
		exit(7);
	}
}

void SendToMobile::SendToMobileEntryPoint() {
	cout << "send msg to mobile" << endl;

	connectToMobile();

	// =========================================================
	// to mobile
	// =========================================================
	// prepare header
	int len;
	char out_len[4];	// holds int describing length of message
	len = htonl(4);		// send 4 char (1 int) network byte order
	memcpy(&out_len, &len, 4);
	// send header: 
	send(mobileSocket, (const char*) out_len, 4, 0);

	// prepare payload
	char toMobile[4];
	messageToMobile = htonl(messageToMobile);
	memcpy(&toMobile, &messageToMobile, sizeof(int));
	// send payload
	send(mobileSocket, (const char*) toMobile, 4, 0);

	// =========================================================
	// receive ack
	// =========================================================
	// read header
	int in_len;
	int recvBytes = 0;
	char inBuffer[4];
	
	recvBytes = recv(mobileSocket, inBuffer, sizeof inBuffer, MSG_WAITALL);
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
			
	// =========================================================
	// read payload

	char* inMessage = new char[in_len];
	recvBytes = recv(mobileSocket, inMessage, in_len, MSG_WAITALL);
	if(recvBytes != in_len) {
		printf("Error: recv returned: %d\n", recvBytes);
	}
	else {
		for(int i = 0; i < in_len; i++) {
			printf("inMsgBuffer %d\n", inMessage[i]);
		}
	}

	// =========================================================
	// finish, close

	closesocket(mobileSocket);
	cout << "socket to Mobile closed" << endl;
	delete inMessage;
}

void SendToMobile::sendMessageToMobile(int msg) {
	messageToMobile = msg;
}

void activateMarker(int markerID) {
	// prepare message to send to mobile and start thread
	SendToMobile* msgToMobile = new SendToMobile();
	msgToMobile->sendMessageToMobile(1);
	msgToMobile->mobileIP = markers[markerID].connectInfoMobile.sin_addr.S_un.S_addr;
	msgToMobile->mobilePort = 8080;
	AfxBeginThread(SendToMobile::SendToMobileStaticEntryPoint, (void*)msgToMobile);
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
	local.sin_port = htons(listenPort);

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

		TcpRequestThread* request = new TcpRequestThread();
		request->setSocket(AcceptSocket, from);
		AfxBeginThread(TcpRequestThread::TcpRequestThreadStaticEntryPoint, (void*)request);

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

	markers[4648].active = false; // 1228
	markers[7236].active = false; // 1c44
	markers[2884].active = false; // 0b44
	markers[1680].active = false; // 0690
	markers[  90].active = false; // 005a
	markers[ 626].active = false; // 0272
	
	int width = 640;
	int height = 480;

	int mouse = ((argc > 1) && (std::string("-m") == argv[1]));
	mouse = 0;
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
		MyImage* img = new MyImage( 
			tmp->width(1)/5, 
			tmp->height(1)/5,
			(int)(((double)random()/(double)RAND_MAX)*700-350),
			(int)(((double)random()/(double)RAND_MAX)*450-225),
			(int)(((double)random()/(double)RAND_MAX)*360),
			tmp, 0xFF
		);
		win->add( img );
	}

	
	textures[0] = new RGBATexture( "img00042.png" );
	textures[1] = new RGBATexture( "img00052.png");
	textures[2] = new RGBATexture( "img00054.png" );
	
	win->update();
	// keep looping on window thread
	win->run();

}
