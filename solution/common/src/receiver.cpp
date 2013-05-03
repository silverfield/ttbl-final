/*****************************************************************/
/* Includes
/*****************************************************************/

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>

#include "commondefs.h"
#include "common.h"
#include "logger.h"
#include "commonlvls.h"

#include "receiver.h"

using namespace std;

/*****************************************************************/
/* Class - Constructors
/*****************************************************************/

Receiver::Receiver(int iPort, int iMaxSize) {
	cPort = iPort;
	cMaxSize = iMaxSize;

	//create socket
	cSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (cSocket < 0) {
		ERR("Receiver::Receiver: Error opening socket" << endl);
	}
	DEB(LVL_RECEIVER, "Receiver::Receiver: Socket " << cSocket << " created successfully" << endl);

	//bind socket to address
	struct sockaddr_in lAddr;
	int lLength = sizeof(lAddr);
	bzero(&lAddr,lLength);
	lAddr.sin_family = AF_INET;
	lAddr.sin_addr.s_addr = INADDR_ANY;
	lAddr.sin_port = htons(iPort);
	if (bind(cSocket,(struct sockaddr *)&lAddr, lLength) < 0) {
		ERR("Receiver::Receiver: Error binding socket" << endl);
	}
	DEB(LVL_RECEIVER, "Receiver::Receiver: Socket " << cSocket << " bound successfully" << endl);
}

Receiver::~Receiver() {
	//close the socket
	close(cSocket);
	DEB(LVL_RECEIVER, "Receiver::~Receiver: Closing socket and destroying myself" << endl);
}

/*****************************************************************/
/* Interface
/*****************************************************************/

string Receiver::fReceive() {
	DEBF(LVL_RECEIVER, "Receiver::fReceive: Attempting to receive at port %d\n", cPort);

	//listen on the socket
	struct sockaddr_in lFromAddr;
	char lBuffer [cMaxSize];
	bzero(lBuffer, cMaxSize);
	socklen_t lFromLength;
	lFromLength = sizeof(struct sockaddr_in);
	int lRecBytes = recvfrom(cSocket, lBuffer, cMaxSize, 0, (struct sockaddr *)&lFromAddr, &lFromLength);
	if (lRecBytes < 0) {
		ERR("Receiver::fReceive: Error receiving" << endl);
		return "";
	}

	char lDiagBuffer [cMaxSize];
	bzero(lDiagBuffer, cMaxSize);
	memcpy(lDiagBuffer, lBuffer, cMaxSize);
	lDiagBuffer[strlen(lDiagBuffer) - 1] = '\0';

	DEBF(LVL_RECEIVER, "Receiver::fReceive: Message received:\n");
	DEBF(LVL_RECEIVER, "	from socket/port: %d/%d\n", cSocket, cPort);
	DEBF(LVL_RECEIVER, "	# of bytes: %d\n", lRecBytes);
	DEBF(LVL_RECEIVER, "	message: \"%s\"\n", lDiagBuffer);

	string lMessage = fCharArrayToString(lBuffer);

	return lMessage;
}
