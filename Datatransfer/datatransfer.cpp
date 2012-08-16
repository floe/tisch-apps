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
#include <fstream>
#include <sstream>
#include "BlobMarker.h"
#include "BlobCount.h"
#include "BlobPos.h"

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

	void deleteMe() {
		parent->remove(this);
		delete this;
	}
};

struct MarkerID {
	int markerID;
	TcpRequestThread* thread;
	sockaddr_in connectInfoMobile;
	bool active;
	HandyDropZone* hdz;
	vector<Container*> imageVector;
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

class InteractionArea: public Container {
public:
	InteractionArea(int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0x00):
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

void showImage(int markerID)
{
	cout << textures[0]->width(1) << " " << textures[0]->height(1) << endl;
	textures[0] = new RGBATexture( "target.png" );//new RGBATexture( "target.png" );
	cout << textures[0]->width(1) << " " << textures[0]->height(1) << endl;
	textures[0]->
	RGBATexture* picture = textures[0]; //new RGBATexture( "target.png" );
	
	MyImage* img = new MyImage(
		200,
		150,
		markers[markerID].hdz->x,
		markers[markerID].hdz->y < 0 ? markers[markerID].hdz->y + markers[markerID].hdz->h : markers[markerID].hdz->y - markers[markerID].hdz->h,
		0,
		picture, 0x05
		);
	//textures[0]
	win->add( img );
	markers[markerID].imageVector.push_back( img );
	
			
	/*cout << "imageSize " << markers[markerID].imageSize << endl;
	cout << "#imgs " << markers[markerID].imageVector.size() << endl;
	unsigned char* imagedata = markers[markerID].imageVector.back();
	
	cout << "open file ... ";
	ofstream outfile("tmp.jpg", ios::out | ios::binary);
	if(!outfile) {
		cout<<"Cannot open output file\n";
		return 1;
	}
	cout << "writing jpg to disk ... ";
	int n[5] = {1, 2, 3, 4, 5};
	outfile.write((char*)&n, sizeof n);
	
	cout << "done ... ";
	outfile.close();
	cout << "close" << endl;
	*/
	
	//RGBATexture* texture = new RGBATexture("tmp.png");
	//MyImage* img = new MyImage( 
	//	200,//tmp->width(1)/5, 
	//	100, //tmp->height(1)/5,
	//	markers[markerID].hdz->x ,
	//	markers[markerID].hdz->y < 0 ? markers[markerID].hdz->y + markers[markerID].hdz->h : markers[markerID].hdz->y - markers[markerID].hdz->h,
	//	0,
	//	texture, 0x05
	//);
	//win->add( img );
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
	cout << "header: " << recvBytes << " bytes; value: " << in_len << endl;

	//===================================================================
	// read payload
	char* inMsgBuffer = new char[in_len];
	recvBytes = recv(socket, inMsgBuffer, in_len, MSG_WAITALL);

	cout << "in_len: " << in_len << " recvBytes: " << recvBytes << endl;

	/*if(recvBytes != in_len) {
		printf("Error: recv returned: %d\n", recvBytes);
	}
	else {
		for(int i = 0; i < in_len; i++) {
			printf("bytes inMsgBuffer %d\n", inMsgBuffer[i]);
		}
	}*/

	int contentType;
	memcpy(&contentType, inMsgBuffer, sizeof(int));
	contentType = ntohl(contentType);
	cout << "contentType: " << contentType << endl;
	switch(contentType) {
	case 0: { // request markerID
			cout << "select marker ID" << endl;
			int useMarkerID = -1;
			for(map<int, MarkerID>::iterator it = markers.begin(); it != markers.end(); it++) {
				if(it->second.active == false) {
					useMarkerID = it->first;
					break;
				}
			}
			
			cout << "selected MakerID: " << useMarkerID << endl;

			if(useMarkerID == -1) {
				cout << "no marker ID left, try later again" << endl;
				closesocket(socket);
				break;
			}
			
			markers[useMarkerID].connectInfoMobile = from;

			// prepare message to send to mobile and start thread
			int messageSize = 8;
			SendToMobile* msgToMobile = new SendToMobile();
			unsigned char* msg = new unsigned char[messageSize];
			int contentType = htonl(0); // send free markerID
			msg[0] = (contentType >> 0) & 0xff;
			msg[1] = (contentType >> 8) & 0xff;
			msg[2] = (contentType >> 16) & 0xff;
			msg[3] = (contentType >> 24) & 0xff;
		
			int markerIDnet = htonl(useMarkerID); // msg
			msg[4] = (markerIDnet >> 0) & 0xff;
			msg[5] = (markerIDnet >> 8) & 0xff;
			msg[6] = (markerIDnet >> 16) & 0xff;
			msg[7] = (markerIDnet >> 24) & 0xff;
		
			msgToMobile->sendMessageToMobile(msg, messageSize);
			msgToMobile->mobileIP = markers[useMarkerID].connectInfoMobile.sin_addr.S_un.S_addr;
			msgToMobile->mobilePort = 8080;
			AfxBeginThread(SendToMobile::SendToMobileStaticEntryPoint, (void*)msgToMobile);

			break;
		}
	case 3: { // TISCH received close connection
			cout << "close connection";

			int mMarkerID;
			char cMarkerID[4] = { inMsgBuffer[4], inMsgBuffer[5], inMsgBuffer[6], inMsgBuffer[7] };
			
			memcpy(&mMarkerID, cMarkerID, sizeof(int));
			mMarkerID = ntohl(mMarkerID);
			cout << " to mobile with mMarkerID: " << mMarkerID << endl;
			
			if(mMarkerID != 0) {
				// deactivate and reset connection info
				markers[mMarkerID].active = false;
				markers[mMarkerID].connectInfoMobile = zero_sockaddr_in;
			
				// remove HandyDropZone from display
				markers[mMarkerID].hdz->deleteMe();
				markers[mMarkerID].hdz = NULL;
			}
			break;
		}
	case 20: { // image received
			// | contentType = (int) 20 | markerID | imagedata |
			cout << "image received" << endl;
			int headersize = 8;
			
			int mMarkerID;
			char cMarkerID[4] = { inMsgBuffer[4], inMsgBuffer[5], inMsgBuffer[6], inMsgBuffer[7] };
			memcpy(&mMarkerID, cMarkerID, sizeof(int));
			mMarkerID = ntohl(mMarkerID);
			
			cout << "markerID: " << mMarkerID << endl;

			unsigned char* image = new unsigned char[recvBytes - headersize];
			for(int i = 0; i < recvBytes - headersize; i++) {
				if(i%100000 == 0)
					cout << "i: " << i << endl;
				image[i] = inMsgBuffer[i+headersize];
			}
			
			cout << "open file ... ";
			ofstream outfile("tmp.jpg", ios::out | ios::binary);
			if(!outfile) {
				cout<<"Cannot open output file\n";
			}
			cout << "writing jpg to disk ... ";
			outfile.write((char*)image, recvBytes - headersize);
			cout << "done ... ";
			outfile.close();
			cout << "close" << endl;
			
			cout << "convert jpg to png" << endl;
			system("convert tmp.jpg target.png");
			cout << "resizing png" << endl;
			system("mogrify -resize 15% target.png");
			cout << "delete tmp.jpg" << endl;
			system("del tmp.jpg");

			showImage(mMarkerID);

			//memcpy(&image, &inMsgBuffer+headersize, recvBytes-headersize);
			
			//markers[mMarkerID].imageSize = recvBytes - headersize;
			
			//cout << "imageSize " << markers[markerID].imageSize << endl;
			//cout << "#imgs " << markers[markerID].imageVector.size() << endl;
			//unsigned char* imagedata = markers[markerID].imageVector.back();
	
			

			

			//
			//cout << "showImage done" << endl;
			/*int mImageID;
			char cImageId[4] = { inMsgBuffer[8], inMsgBuffer[9], inMsgBuffer[10], inMsgBuffer[11] };
			memcpy(&mImageID, cImageId, sizeof(int));
			mImageID = ntohl(mImageID);
			
			cout << "markerID: " << mMarkerID << " image id: " << mImageID << endl;

			showImage(mMarkerID, mImageID);*/
			

			break;
		}
	default: {
			cout << "default" << endl;
			break;
		}
	}

	cout << "delete inMsgBuffer" << endl;
	delete inMsgBuffer;

	cout << "close socket ... ";
	closesocket(socket);

	cout << "done" << endl;
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
	const int headerSize = 4; // byte
	int len = htonl(messageSize); // transform messageSize to network
	char out_len[headerSize];	// holds int describing length of message
	memcpy(&out_len, &len, headerSize);
	// send header: 
	cout << "send length" << endl;
	send(mobileSocket, (const char*) out_len, headerSize, 0);

	// send payload
	cout << "send data ... ";
	send(mobileSocket, (const char*) messageToMobile, messageSize, 0);

	// =========================================================
	// finish, close

	closesocket(mobileSocket);
	cout << "done, close" << endl;
	
}

void SendToMobile::sendMessageToMobile(unsigned char* msg, int amountOfBytes) {
	cout << "set msg to send" << endl;
	cout << "bytes to send: " << amountOfBytes << endl;
	messageToMobile = msg;
	messageSize = amountOfBytes;
}

void activateMarker(int markerID) {
	cout << "activate marker: " << markerID << endl;
	// prepare message to send to mobile and start thread
	int messageSize = 8;
	SendToMobile* msgToMobile = new SendToMobile();
	unsigned char* msg = new unsigned char[messageSize];
	int contentType = htonl(1); // foundMarker
	msg[0] = (contentType >> 0)& 0xff;
	msg[1] = (contentType >> 8) & 0xff;
	msg[2] = (contentType >> 16) & 0xff;
	msg[3] = (contentType >> 24) & 0xff;
		
	int markerIDnet = htonl(markerID); // msg
	msg[4] = (markerIDnet >> 0) & 0xff;
	msg[5] = (markerIDnet >> 8) & 0xff;
	msg[6] = (markerIDnet >> 16) & 0xff;
	msg[7] = (markerIDnet >> 24) & 0xff;
		
	msgToMobile->sendMessageToMobile(msg, messageSize);
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

	InteractionArea* iArea = new InteractionArea( width, height );
	iArea->add( new Label("interaction area", 300, 20, 0, 120, 0, 1) );
	win->add(iArea);

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
