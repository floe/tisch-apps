/*************************************************************************\
*    Datatransfer - libTISCH demo - see http://tisch.sourceforge.net/     *
*     Copyright (c) 2012 by Norbert Wiedermann, <wiederma@in.tum.de>      *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/
#include "Datatransfer\datatransfer.h"

Window* win = 0;

std::map<int, MarkerID> markers;

void MyImage::setRawTexture(unsigned char* _texture, int _tex_width, int _tex_height) {
	raw_image = _texture;
	load_Tex = 1;
	tex_width = _tex_width;
	tex_height = _tex_height;
}

void MyImage::action( Gesture* gesture ) {
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
					int headerSize = 8;
					int messageSize = headerSize + JPG_data_size;
					cout << "messageSize to send: " << messageSize << endl;
					SendToMobile* msgToMobil = new SendToMobile();
					unsigned char* msg = new unsigned char[messageSize];
					// | contentType = (int) 20 | markerID | imagedata |
					
					int contentType = htonl(20); // send Image
					msg[0] = (contentType >> 0) & 0xff;
					msg[1] = (contentType >> 8) & 0xff;
					msg[2] = (contentType >> 16) & 0xff;
					msg[3] = (contentType >> 24) & 0xff;

					cout << "contentType set" << endl;

					int targetMarkerID = it->first;
					int targetMarkerIDnet = htonl(targetMarkerID);
					msg[4] = (targetMarkerIDnet >> 0) & 0xff;
					msg[5] = (targetMarkerIDnet >> 8) & 0xff;
					msg[6] = (targetMarkerIDnet >> 16) & 0xff;
					msg[7] = (targetMarkerIDnet >> 24) & 0xff;

					cout << "targetMarkerID set" << endl;

					for(int i = 0; i < JPG_data_size; i += 100000) {
						cout << i << " " << JPG_data[i] << endl;
					}
					
					// skip the first 8 chars and save image data thereafter
					memcpy(msg + 8 * sizeof(unsigned char), JPG_data, JPG_data_size * sizeof(unsigned char));
					
					cout << "header data:" << endl;
					for(int i = 0; i < 8; i++) {
						cout << i << " >" << msg[i] << "<" << endl;
					}
					
					cout << "body data:" << endl;
					for(int i = 8; i < messageSize; i += 100000) {
						cout << i << " >" << msg[i] << "<" << endl;
					}

					cout << "message prepared to send to mobile" << endl;

					msgToMobil->sendMessageToMobile(msg, messageSize);
					msgToMobil->mobileIP = markers[targetMarkerID].connectInfoMobile.sin_addr.S_un.S_addr;
					msgToMobil->mobilePort = 8080;
					AfxBeginThread(SendToMobile::SendToMobileStaticEntryPoint, (void*) msgToMobil);

					// update HDZs were image is connected to
					belongsToHDZ.push_back(it->second.hdz);
					cout << "HDZ updated" << endl;

					break;
				}
			}
		}
	}
    else
        Container::action(gesture);
}

void MyImage::draw() {
	enter();

	if( load_Tex == 1) {
		cout << "load Texture" << endl;
		load_Tex = 0;
		cout << "w: " << tex_width << " h: " << tex_height << endl;
		RGBATexture* mytexTmp = new RGBATexture( tex_width, tex_height );
		mytexTmp->load( raw_image, GL_RGB, GL_UNSIGNED_BYTE );
		cout << "free mem ";
		free(raw_image);
		cout << "done" << endl;

		mytex = mytexTmp;
	}
	/*if(imageName != NULL) {
		mytex = new RGBATexture( imageName );
		cout << "MyImage draw load texture ";
		string delTargetPNG = string("del " + string(imageName));
		system(delTargetPNG.c_str());
		cout << "target.png tmp file deleted" << endl;
		imageName = NULL;
	}*/

	Widget::paint();
	paint();
	leave();
}

void InteractionArea::action( Gesture* gesture ) {
		
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
			markers[markerIDtmp].hdz = new HandyDropZone(150,
				200,
				blobPos.x * win->getWidth() * 0.5f,
				blobPos.y * win->getHeight() * 0.5f,
				0.0,
				new RGBATexture( "handy.png" ));

			markers[markerIDtmp].active = true;
			markers[markerIDtmp].hdz->myMarkerID = markerIDtmp;

			win->add(markers[markerIDtmp].hdz);
		}
	} // if( gesture->name() == "handy" )
} // void action( Gesture* gesture )

void TcpRequestThread::setSocket(SOCKET _socket, sockaddr_in _from) {
	socket = _socket;
	from = _from;
}

/**
 * print the information for what was stored in the JPEG File
 **/
void TcpRequestThread::print_jpeg_info(struct jpeg_decompress_struct cinfo) {
    printf("JPEG File Information: \n");
    printf("Image width and height: %d pixels and %d pixels.\n", cinfo.image_width, cinfo.image_height);
    printf("Color components per pixel: %d.\n", cinfo.num_components);
    printf("Color space: %d.\n", cinfo.jpeg_color_space);
    printf("Raw flag is: %d.\n", cinfo.raw_data_out);
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

				// notify all images belong to this HDZ that it leaves
				std::vector<MyImage*> notityImgs = markers[mMarkerID].hdz->imageVector;
				for (int i = 0; i < notityImgs.size(); i++) {
					notityImgs.at(i)->removeHDZ(mMarkerID);
				}
				
				// remove HandyDropZone from display
				markers[mMarkerID].hdz->deleteMe();
				markers[mMarkerID].hdz = NULL;
			}
			break;
		}
	case 20: { // image received
			// | contentType = (int) 20 | markerID | imagedata |
			cout << "image received" << endl;
			int headersize = 8; // 4byte contentType, 4byte markerID
			
			int mMarkerID;
			char cMarkerID[4] = { inMsgBuffer[4], inMsgBuffer[5], inMsgBuffer[6], inMsgBuffer[7] };
			memcpy(&mMarkerID, cMarkerID, sizeof(int));
			mMarkerID = ntohl(mMarkerID);
			
			cout << "markerID: " << mMarkerID << endl;

			unsigned char* jpg_image = new unsigned char[recvBytes - headersize];
			
			// copy jpeg image data only, skipping header
			memcpy(jpg_image, (unsigned char*)&inMsgBuffer[headersize], (recvBytes - headersize)*sizeof(unsigned char));

			/* these are standard libjpeg structures for reading(decompression) */
			struct jpeg_decompress_struct cinfo;
			struct jpeg_error_mgr jerr;
			/* libjpeg data structure for storing one row, that is, scanline of an image */
			JSAMPROW row_pointer[1];
			unsigned long location = 0;
			int i = 0;

			//char* filename;
			//FILE *infile = fopen(filename, "rb");
			//if (!infile) {
			//	printf("Error opening jpeg file %s\n!", filename);
			//}
			
			/* here we set up the standard libjpeg error handler */
			cinfo.err = jpeg_std_error(&jerr);
			/* setup decompression process and source, then read JPEG header */
			jpeg_create_decompress(&cinfo);

			/* this makes the library read from infile */
			//jpeg_stdio_src(&cinfo, infile);

			/* this makes the library read from memory */
			jpeg_mem_src(&cinfo, jpg_image, recvBytes - headersize);

			/* reading the image header which contains image information */
			jpeg_read_header(&cinfo, TRUE);
			print_jpeg_info(cinfo);
			jpeg_start_decompress(&cinfo);

			/* allocate memory to hold the uncompressed image */
			size = cinfo.output_width*cinfo.output_height*cinfo.num_components;
			raw_image = (unsigned char*)malloc(size);
			/* now actually read the jpeg into the raw buffer */
			row_pointer[0] = (unsigned char *)malloc(cinfo.output_width*cinfo.num_components);
			/* read one scan line at a time */
			while (cinfo.output_scanline < cinfo.image_height) {
				jpeg_read_scanlines( &cinfo, row_pointer, 1 );
				for (i=0; i<cinfo.image_width*cinfo.num_components;i++) {
					raw_image[location++] = row_pointer[0][i];
				}
			}
			
			MyImage* my_img = new MyImage(
				200,
				150,
				markers[mMarkerID].hdz->x,
				markers[mMarkerID].hdz->y < 0 ? markers[mMarkerID].hdz->y + markers[mMarkerID].hdz->h : markers[mMarkerID].hdz->y - markers[mMarkerID].hdz->h,
				0,
				NULL, 0x05
				);

			my_img->setRawTexture(raw_image, cinfo.image_width, cinfo.image_height);
			my_img->belongsToHDZ.push_back( markers[mMarkerID].hdz );
			my_img->JPG_data = jpg_image;
			my_img->JPG_data_size = recvBytes - headersize;

			win->add( my_img );

			// save my_img widget
			markers[mMarkerID].hdz->imageVector.push_back( my_img );

			/* wrap up decompression, destroy objects, free pointers */
			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			free(row_pointer[0]);
			//fclose(infile);
			/* yup, we succeeded! */

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
	
	win->update();
	// keep looping on window thread
	win->run();

}
