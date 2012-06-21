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
	bool active;
	HandyDropZone* hdz;
};

Window* win = 0;

std::map<int, MarkerID> markers;


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
					
				markers[markerIDtmp].thread->activateMarker();

				RGBATexture* texture = new RGBATexture( TISCH_PREFIX "Box.png" );
					
				f = (*gesture)[1];
				BlobPos* bp = dynamic_cast<BlobPos*>(f);
				Vector blobPos = bp->result();
				cout << "BlobPos: " << blobPos.x << " " << blobPos.y << endl;
				transform(blobPos,1);
				blobPos.z = 0;
				blobPos.normalize();
				cout << "BlobPos: " << blobPos.x << " " << blobPos.y << endl;
				markers[markerIDtmp].hdz = new HandyDropZone(150, 200, blobPos.x * win->getWidth() * 0.5f, blobPos.y * win->getHeight() * 0.5f, 0.0, texture);
				markers[markerIDtmp].active = true;

				win->add(markers[markerIDtmp].hdz);
			}
		} // if( gesture->name() == "handy" )
	} // void action( Gesture* gesture )
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

	// =========================================================
	// to client
	// =========================================================
	char out_len[4];
	char* toClient;
	int len, mID_endian;

	len = htonl(4);
	memcpy(&out_len, &len, 4);
	send(socket, (const char*) out_len, 4, 0);

	// send markerID
	//char toClient[4];
	toClient = new char[4];
	mID_endian = htonl(markerID);
	memcpy(toClient, &mID_endian, sizeof(int));
	send(socket, (const char*)toClient, 4, 0);

	//char temp[512];
	//sprintf(temp, "Your IP is %s\r\n",inet_ntoa(from.sin_addr));
	//send(socket, temp, strlen(temp), 0);
	//cout << "Connection from " << inet_ntoa(from.sin_addr) << endl;
	
	closesocket(socket);

	cout << "socket closed" << endl;
}
/*
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
}*/

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
	len = htonl(4);		// send 4 char
	memcpy(&out_len, &len, 4);
	// send header: 
	send(mobileSocket, (const char*) out_len, 4, 0);

	// prepare payload
	char toMobile[4];
	messageToMobile = htonl(messageToMobile);
	memcpy(toMobile, &messageToMobile, sizeof(int));
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

void TcpRequestThread::activateMarker() {
	// prepare message to send to mobile and start thread
	SendToMobile* msgToMobile = new SendToMobile();
	msgToMobile->sendMessageToMobile(1);
	AfxBeginThread(SendToMobile::SendToMobileStaticEntryPoint, (void*)msgToMobile);

	/*char out_len[4];
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
	protocolStep++;*/
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

		int useMarkerID = -1;

		for(map<int, MarkerID>::iterator it = markers.begin(); it != markers.end(); it++) {
			if(it->second.thread == NULL) {
				useMarkerID = it->first;
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

			markers[useMarkerID].thread = request;
		
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

	win->update();
	// keep looping on window thread
	win->run();

}
