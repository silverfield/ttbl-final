/* Implements receiving on port */

#ifndef RECEIVER_H_
#define RECEIVER_H_

/*****************************************************************/
/* Includes
/*****************************************************************/

#include <string.h>

/*****************************************************************/
/* Class
/*****************************************************************/

class Receiver {
private:
	//----- data -----
	int cSocket;
	int cPort;
	int cMaxSize;
public:
	//----- constructors -----
	Receiver(int iPort, int iMaxSize);
	~Receiver();

	//----- interface -----
	std::string fReceive();

};

#endif /* RECEIVER_H_ */
