/*****************************************************************/
/* Includes
/*****************************************************************/

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <stdio.h>

#include "logger.h"
#include "commondefs.h"
#include "commonlvls.h"

#include "sender.h"

using namespace std;

/*****************************************************************/
/* Class - Constructors
/*****************************************************************/

Sender::Sender(int iPort, int iMaxSize) {
	cPort = iPort;
	cMaxSize = iMaxSize;

	//resolve hostname
	struct hostent *lHost;
	lHost = gethostbyname("localhost");
	if (lHost == 0) {
		DEB(LVL_SENDER, "Sender::Sender: Error resolving hostname" << endl);
	}
	DEB(LVL_SENDER, "Sender::Sender: Hostname resolved" << endl);

	//create socket
	cSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (cSocket < 0) {
		DEB(LVL_SENDER, "Sender::Sender: Error creating socket" << endl);
	}
	DEB(LVL_SENDER, "Sender::Sender: Socket " << cSocket << " created successfully" << endl);

	//create the server address structure
	cServerAddr.sin_family = AF_INET;
	bcopy((char *)lHost->h_addr, (char *)&cServerAddr.sin_addr, lHost->h_length);
	cServerAddr.sin_port = htons(iPort);
}

Sender::~Sender() {
	//close the socket
	close(cSocket);
	DEB(LVL_SENDER, "Sender::~Sender: Closing socket and destroying myself" << endl);
}

/*****************************************************************/
/* Interface
/*****************************************************************/

Ret Sender::fSend(string iMessage) {
	for (int i = 0; i < iMessage.size(); i += (cMaxSize - 1)) {
		string lPart = iMessage.substr(i, cMaxSize - 1);
		Ret lRetSend = fSendPart(lPart);
		if (lRetSend != OK) {
			DEB(LVL_SENDER, "Sender::fSend: Error sending part of the message" << endl);
			return NOK;
		}
	}

	return OK;
}

/*****************************************************************/
/* Implementation
/*****************************************************************/

Ret Sender::fSendPart(string iMessage) {
	//prepare message, we truncate the message to max-size bytes
	int lMsgSize = min((int)iMessage.size() + 1, cMaxSize);
	char lBuffer[lMsgSize];
	bzero(lBuffer, lMsgSize);
	memcpy(lBuffer, iMessage.c_str(), lMsgSize);
	if (iMessage[iMessage.size() - 1] != '\n') {
		strcat(lBuffer, "\n");
	}
	unsigned int lAddrLength;
	lAddrLength = sizeof(struct sockaddr_in);

	//send message
	int lSentBytes = sendto(cSocket, lBuffer, strlen(lBuffer) + 1, 0, (const struct sockaddr *)&cServerAddr, lAddrLength);
	if (lSentBytes < 0) {
		DEB(LVL_SENDER, "Sender::fSendPart: Error sending message" << endl);
		return NOK;
	}

	lBuffer[strlen(lBuffer) - 1] = '\0';

	DEBF(LVL_SENDER, "Sender::fSendPart: Message sent:\n");
	DEBF(LVL_SENDER, "	from socket/port: %d/%d\n", cSocket, cPort);
	DEBF(LVL_SENDER, "	# of bytes: %d\n", lSentBytes);
	DEBF(LVL_SENDER, "	message: \"%s\"\n", lBuffer);

	return OK;
}
