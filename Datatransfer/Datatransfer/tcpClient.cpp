/*************************************************************************\
*    Datatransfer - libTISCH demo - see http://tisch.sourceforge.net/     *
*     Copyright (c) 2012 by Norbert Wiedermann, <wiederma@in.tum.de>      *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/
#include "Socket.h"

#include <iostream>

using namespace std;

class TcpClient {
public: TcpClient() {

		// Socket erstellen
		int socks = socket(AF_INET, SOCK_STREAM, 0);
		// Verbindungsziel festlegen, Port und IP-Adresse des Servers angeben
		struct sockaddr_in serveraddr;
		memset(&serveraddr, 0, sizeof(serveraddr));
		inet_pton(AF_INET, "127.0.0.1", &(serveraddr.sin_addr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(1111);
 
		// Verbindung aufbauen
		connect(socks, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
 
		// Puffer mit Text füllen
		char buffer[5];
		strcpy(buffer, "test");
 
		// "test" String an Server senden
		int bytes = send(socks , buffer, sizeof(buffer), 0);
 
		// Ausgabe was gesendet wurde
		printf("Sende String: %s\n", buffer);
		printf("Es wurden %d Bytes gesendet\n", bytes);
 
		// Verbindung beenden
		close(socks);
	}
};