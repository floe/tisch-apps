/*************************************************************************\
* part of Datatransfer - libTISCH demo - see http://tisch.sourceforge.net/*
*     Copyright (c) 2012 by Norbert Wiedermann, <wiederma@in.tum.de>      *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "tcpServer.h"

class TcpServer {
public: TcpServer() {
	// Socket erstellen - TCP, IPv4, keine Optionen
	int lsd = socket(AF_INET, SOCK_STREAM, 0);
 
	// IPv4, Port: 1111, jede IP-Adresse akzeptieren
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(1111);
	saddr.sin_addr.s_addr = htons(INADDR_ANY);
 
	// Socket an Port binden
	bind(lsd, (struct sockaddr*) &saddr, sizeof(saddr));
 
	// Auf Socket horchen (Listen)
	listen(lsd, 10);
 
	while(1) {
		// Puffer und Strukturen anlegen
		struct sockaddr_in clientaddr;
		char buffer[10];
		memset(buffer, 0, sizeof(buffer));
 
		// Auf Verbindung warten, bei Verbindung Connected-Socket erstellen
		socklen_t clen = sizeof(clientaddr);
		int csd = accept(lsd, (struct sockaddr*) &clientaddr, &clen);
 
		// Vom Client lesen und ausgeben
		int bytes = recv(csd, buffer, sizeof(buffer), 0);
		printf("Der Client hat folgenden String gesendet: %s\n", buffer);
		printf("Es wurden %d Bytes empfangen\n", bytes);
 
		// Verbindung schlieﬂen
		close(csd);
	}
 
	close(lsd);
	
	}
};
