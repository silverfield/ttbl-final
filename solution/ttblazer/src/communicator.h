/* Communicate (receive messages) in a separate thread with Commander application */

#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_

/*****************************************************************/
/* Constants
/*****************************************************************/

/*****************************************************************/
/* Class declaration
/*****************************************************************/

class Communicator {
private:
	//----- data -----
	in_port_t cPort;

	//----- static implementation -----
	static void* fStartCommunicator(void *iCommunicator);

	//----- implementation -----
	void fCommunicate();
public:
	//----- constructors -----
	Communicator(int iPort);

	//----- static interface -----
	static Communicator* fCreateCommunicator(int iPort);
};

#endif
