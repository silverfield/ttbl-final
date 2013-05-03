/* Implements sending on port */

#ifndef SENDER_H_
#define SENDER_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string.h>
#include <netinet/in.h>

#include "commondefs.h"

/*****************************************************************/
/* Class
/*****************************************************************/

class Sender {
private:
	//----- data -----
	int cSocket;
	int cPort;
	struct sockaddr_in cServerAddr;
	int cMaxSize;

	//----- implementation -----
	Ret fSendPart(std::string iMessage);
public:
	//----- constructors -----
	Sender(int iPort, int iMaxSize);
	~Sender();

	//----- interface -----
	Ret fSend(std::string iMessage);

	//----- inline -----
	int fGetPort() {return cPort;};
};

#endif /* SENDER_H_ */
